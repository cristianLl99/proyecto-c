#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "socio.h"
#include "cadena.h"
#include "util.h"
#include "auditoria.h"
#include "config.h"
#include "menu.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    convierte una linea CSV de 10 campos en t_socio. Los campos
                con formato invalido quedan con valores que haran fallar la
                validacion correspondiente (dni = 0, fecha 0/0/0, sexo '?',
                etc.). El estado se fija en 'A'.
Parametros:     linea: linea CSV.
                reg: puntero a t_socio destino.
Retorno:        OK si la cantidad de campos es correcta, ERROR en caso
                contrario.
Observaciones:  Se pasa por puntero como parsear a leer_archivo_texto.
******************************************************************************/
int texto_a_socio_ALU(const char *linea, void *reg)
{
    t_socio *s = (t_socio *)reg;
    char     buf[TAM_LINEA];
    char    *campos[CAMPOS_SOCIO];

    memset(s, 0, sizeof(t_socio));
    s->estado = 'A';

    copiar_cadena(buf, linea, TAM_LINEA);
    quitar_salto(buf);

    if(separar_campos_ALU(buf, campos, CAMPOS_SOCIO) != CAMPOS_SOCIO)
        return ERROR;

    s->dni = es_numero(campos[0]) ? strtol(campos[0], NULL, 10) : 0;

    if(strlen(campos[1]) < TAM_CUIL)
        copiar_cadena(s->cuil, campos[1], TAM_CUIL);

    copiar_cadena(s->apellidos_nombres, campos[2], TAM_APYN);

    if(texto_a_fecha(campos[3], &s->fecha_nacimiento) != OK)
    {
        s->fecha_nacimiento.dia = 0;
        s->fecha_nacimiento.mes = 0;
        s->fecha_nacimiento.anio = 0;
    }

    s->sexo = (strlen(campos[4]) == 1) ? campos[4][0] : '?';

    if(texto_a_fecha(campos[5], &s->fecha_afiliacion) != OK)
    {
        s->fecha_afiliacion.dia = 0;
        s->fecha_afiliacion.mes = 0;
        s->fecha_afiliacion.anio = 0;
    }

    copiar_cadena(s->categoria, campos[6], TAM_CATEGORIA);

    if(texto_a_fecha(campos[7], &s->fecha_ultima_cuota) != OK)
    {
        s->fecha_ultima_cuota.dia = 0;
        s->fecha_ultima_cuota.mes = 0;
        s->fecha_ultima_cuota.anio = 0;
    }

    copiar_cadena(s->plan, campos[8], TAM_PLAN);

    if(strlen(campos[9]) <= EMAIL_MAX_LEN)
        copiar_cadena(s->email_tutor, campos[9], TAM_EMAIL);
    else
        strcpy(s->email_tutor, "@");

    return OK;
}

/******************************************************************************
Descripcion:    aplica las validaciones del punto 6.1 en orden (DNI, Sexo,
                CUIL, Apellidos y Nombres, Fecha de nacimiento, Fecha de
                afiliacion, Categoria, Fecha de ultima cuota, Plan, Email
                tutor). Normaliza apellidos y nombres. Se detiene en el primer
                error.
Parametros:     reg: puntero a t_socio a validar (se modifica al normalizar).
                ctx: contexto con indice, archivo binario, fecha de proceso e
                id_modificando.
                tipo_error: salida; nombre de la validacion que fallo.
Retorno:        OK si el socio es valido, ERROR en caso contrario.
Observaciones:  Si ctx->id_modificando == dni no se considera duplicado y se
                excluye ese DNI al verificar unicidad del email. Cumple con el
                puntero validar de leer_archivo_texto.
******************************************************************************/
int valida_socio_ALU(void *reg, const t_contexto *ctx, char *tipo_error)
{
    t_socio     *s = (t_socio *)reg;
    t_reg_indice reg_ind;
    char         cuil_calculado[TAM_CUIL];
    int          edad_socio;

    reg_ind.id = s->dni;
    reg_ind.nro_reg = 0;
    if(s->dni <= DNI_MIN || s->dni >= DNI_MAX ||
       (s->dni != ctx->id_modificando &&
        indice_buscar_ALU(ctx->indice, &reg_ind, sizeof(t_reg_indice),
                          compara_reg_indice_ALU) != NO_EXISTE))
    {
        strcpy(tipo_error, "DNI");
        return ERROR;
    }

    if(s->sexo != 'H' && s->sexo != 'M')
    {
        strcpy(tipo_error, "SEXO");
        return ERROR;
    }

    calcular_cuil_ALU(s->dni, s->sexo, cuil_calculado);
    if(strcmp(s->cuil, cuil_calculado) != 0)
    {
        strcpy(tipo_error, "CUIL");
        return ERROR;
    }

    arreglar_apyn_ALU(s->apellidos_nombres);
    if(cadena_vacia(s->apellidos_nombres))
    {
        strcpy(tipo_error, "APELLIDOS_NOMBRES");
        return ERROR;
    }

    if(!fecha_correcta(&s->fecha_nacimiento) ||
       compara_fechas(&s->fecha_nacimiento, ctx->fecha_proceso) >= 0 ||
       edad(&s->fecha_nacimiento, ctx->fecha_proceso) < EDAD_MINIMA)
    {
        strcpy(tipo_error, "FECHA_NACIMIENTO");
        return ERROR;
    }
    edad_socio = edad(&s->fecha_nacimiento, ctx->fecha_proceso);

    if(!fecha_correcta(&s->fecha_afiliacion) ||
       compara_fechas(&s->fecha_afiliacion, &s->fecha_nacimiento) <= 0 ||
       compara_fechas(&s->fecha_afiliacion, ctx->fecha_proceso) > 0)
    {
        strcpy(tipo_error, "FECHA_AFILIACION");
        return ERROR;
    }

    if((edad_socio <  EDAD_ADULTO && strcmp(s->categoria, CAT_MENOR)  != 0) ||
       (edad_socio >= EDAD_ADULTO && strcmp(s->categoria, CAT_ADULTO) != 0))
    {
        strcpy(tipo_error, "CATEGORIA");
        return ERROR;
    }

    if(!fecha_correcta(&s->fecha_ultima_cuota) ||
       compara_fechas(&s->fecha_ultima_cuota, &s->fecha_afiliacion) < 0 ||
       compara_fechas(&s->fecha_ultima_cuota, ctx->fecha_proceso) > 0)
    {
        strcpy(tipo_error, "FECHA_ULTIMA_CUOTA");
        return ERROR;
    }

    if(plan_indice(s->plan) == -1)
    {
        strcpy(tipo_error, "PLAN");
        return ERROR;
    }

    if(strcmp(s->categoria, CAT_MENOR) == 0)
    {
        if(!email_valido_ALU(s->email_tutor) ||
           socio_email_existe(ctx->arch_bin, ctx->indice, s->email_tutor,
                              ctx->id_modificando))
        {
            strcpy(tipo_error, "EMAIL_TUTOR");
            return ERROR;
        }
    }
    else if(!cadena_vacia(s->email_tutor))
    {
        strcpy(tipo_error, "EMAIL_TUTOR");
        return ERROR;
    }

    return OK;
}

/******************************************************************************
Descripcion:    lee el registro nro_reg del archivo binario de socios.
Parametros:     arch: archivo abierto en modo binario.
                nro_reg: numero de registro.
                s: socio destino.
Retorno:        OK si se pudo leer, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socio_leer_ALU(FILE *arch, unsigned nro_reg, t_socio *s)
{
    long desplazamiento = (long) nro_reg * sizeof(t_socio);
    if (fseek(arch, desplazamiento, SEEK_SET) != 0)
        {
            return ERROR;
        }
    if (fread(s, sizeof(t_socio), 1, arch) != 1)
        {
            return ERROR;
        }
    return OK;
}

/******************************************************************************
Descripcion:    escribe el socio en la posicion nro_reg del archivo binario y
                hace flush.
Parametros:     arch: archivo abierto en modo de escritura binaria.
                nro_reg: numero de registro.
                s: socio a grabar.
Retorno:        OK si se pudo grabar, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socio_escribir_ALU(FILE *arch, unsigned nro_reg, const t_socio *s)
{
    long desplazamiento = (long) nro_reg * sizeof(t_socio);
    if (fseek(arch, desplazamiento, SEEK_SET) != 0)
        {
            return ERROR;
        }
    if (fwrite(s, sizeof(t_socio), 1, arch) != 1)
        {
            return ERROR;
        }
    fflush(arch);
    return OK;
}

/******************************************************************************
Descripcion:    aplica la regla del Anexo A.4: el socio esta al dia si la
                ultima cuota es del mes de la fecha de proceso, o del mes
                anterior cuando el dia de la fecha de proceso es <= 10.
Parametros:     ultima_cuota: fecha de ultima cuota paga.
                fecha_proceso: fecha de proceso.
Retorno:        1 si esta al dia, 0 en caso contrario.
Observaciones:  -
******************************************************************************/
int cuota_al_dia_ALU(const t_fecha *ultima_cuota, const t_fecha *fecha_proceso)
{
    if (ultima_cuota->anio == fecha_proceso->anio && ultima_cuota->mes == fecha_proceso->mes)
    {
        return 1;
    }
    if (fecha_proceso->dia <= 10)
    {
        int MesAnterior = fecha_proceso->mes - 1;
        int AnioAnterior = fecha_proceso->anio;

        if (MesAnterior == 0)
        {
            MesAnterior = 12;
            AnioAnterior--;
        }

        if (ultima_cuota->anio == AnioAnterior && ultima_cuota->mes == MesAnterior)
        {
            return 1;
        }
    }
    return 0;
}

/******************************************************************************
Descripcion:    opcion Alta: carga un socio por teclado, lo valida con
                valida_socio, lo agrega al final de socios.dat con estado 'A'
                y lo inserta en el indice. Si es rechazado lo audita.
Parametros:     indice: indice de socios.
                path: ruta de socios.dat.
                fecha_proceso: fecha de proceso.
Retorno:        OK si el socio fue agregado, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socio_alta_manual_ALU(t_indice *indice, const char *path, const t_fecha *fecha_proceso)
{
    FILE *arch;
    t_socio socio;
    t_contexto ctx;
    t_reg_indice reg_ind;
    char tipo_error[TAM_TIPO_ERROR];
    char linea[128];
    char dni_txt[32];
    long pos;
    int ret = ERROR;

    arch = fopen(path, "r+b");
    if (!arch)
    {
        arch = fopen(path, "w+b");
        if (!arch)
        {
            printf("Error: no se pudo abrir ni crear el archivo de socios.\n");
            return ERROR;
        }
    }

    ctx.indice = indice;
    ctx.arch_bin = arch;
    ctx.fecha_proceso = fecha_proceso;
    ctx.id_modificando = 0;

    memset(&socio, 0, sizeof(t_socio));
    tipo_error[0] = '\0';

    printf("\n--- ALTA MANUAL DE SOCIO ---\n");

    printf("DNI (entre %ld y %ld): ", (long)DNI_MIN, (long)DNI_MAX);
    fgets(linea, sizeof(linea), stdin);
    if (sscanf(linea, "%ld", &socio.dni) != 1)
    {
        printf("Error en el formato del DNI.\n");
        goto fin;
    }

    printf("CUIL (XY-NNNNNNNN-Z): ");
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    strncpy(socio.cuil, linea, sizeof(socio.cuil) - 1);
    socio.cuil[sizeof(socio.cuil) - 1] = '\0';

    printf("Apellidos y nombres: ");
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    strncpy(socio.apellidos_nombres, linea, sizeof(socio.apellidos_nombres) - 1);
    socio.apellidos_nombres[sizeof(socio.apellidos_nombres) - 1] = '\0';

    printf("Fecha de nacimiento (DD/MM/AAAA): ");
    fgets(linea, sizeof(linea), stdin);
    if (sscanf(linea, "%d/%d/%d", &socio.fecha_nacimiento.dia,
               &socio.fecha_nacimiento.mes, &socio.fecha_nacimiento.anio) != 3)
    {
        printf("Error en el formato de la fecha de nacimiento.\n");
        goto fin;
    }

    printf("Sexo (H/M): ");
    fgets(linea, sizeof(linea), stdin);
    socio.sexo = (char)toupper((unsigned char)linea[0]);

    printf("Fecha de afiliacion (DD/MM/AAAA): ");
    fgets(linea, sizeof(linea), stdin);
    if (sscanf(linea, "%d/%d/%d", &socio.fecha_afiliacion.dia,
               &socio.fecha_afiliacion.mes, &socio.fecha_afiliacion.anio) != 3)
    {
        printf("Error en el formato de la fecha de afiliacion.\n");
        goto fin;
    }

    printf("Categoria (%s/%s): ", CAT_MENOR, CAT_ADULTO);
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    strncpy(socio.categoria, linea, sizeof(socio.categoria) - 1);
    socio.categoria[sizeof(socio.categoria) - 1] = '\0';


    printf("Fecha de ultima cuota paga (DD/MM/AAAA): ");
    fgets(linea, sizeof(linea), stdin);
    if (sscanf(linea, "%d/%d/%d", &socio.fecha_ultima_cuota.dia,
               &socio.fecha_ultima_cuota.mes, &socio.fecha_ultima_cuota.anio) != 3)
    {
        printf("Error en el formato de la fecha de ultima cuota.\n");
        goto fin;
    }

    socio.estado = 'A';

    printf("Plan (BASIC/PREMIUM/VIP/FAMILY): ");
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    strncpy(socio.plan, linea, sizeof(socio.plan) - 1);
    socio.plan[sizeof(socio.plan) - 1] = '\0';

    printf("Email tutor (vacio si ADULTO): ");
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    strncpy(socio.email_tutor, linea, sizeof(socio.email_tutor) - 1);
    socio.email_tutor[sizeof(socio.email_tutor) - 1] = '\0';

    if (valida_socio_ALU(&socio, &ctx, tipo_error) == OK)
    {
        fseek(arch, 0, SEEK_END);
        pos = ftell(arch);

        if (fwrite(&socio, sizeof(t_socio), 1, arch) != 1)
        {
            printf("Error al escribir el socio.\n");
            goto fin;
        }
        fflush(arch);

        reg_ind.nro_reg = (unsigned)(pos / (long)sizeof(t_socio));
        reg_ind.id = clave_socio(&socio);
        indice_insertar_ALU(indice, &reg_ind, sizeof(t_reg_indice), compara_reg_indice_ALU);

        printf("\nSocio dado de alta exitosamente.\n");
        ret = OK;
    }
    else
    {
        sprintf(dni_txt, "%ld", socio.dni);   /* la auditoria guarda el DNI */
        rechaza_socio(&socio, dni_txt, tipo_error, fecha_proceso);
        printf("\nSocio RECHAZADO. Validacion fallida: %s\n", tipo_error);
    }

fin:
    fclose(arch);
    return ret;
}

/******************************************************************************
Descripcion:    opcion Modificacion: pide el DNI, lo busca en el indice (un
                socio dado de baja no se puede modificar), muestra los datos
                actuales, pide los nuevos (Enter conserva), valida y
                sobrescribe el registro. El DNI y el estado no se modifican.
                Si es rechazado lo audita.
Parametros:     indice: indice de socios.
                path: ruta de socios.dat.
                fecha_proceso: fecha de proceso.
Retorno:        OK si se modifico, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socio_modificacion_manual_ALU(t_indice *indice, const char *path, const t_fecha *fecha_proceso)
{
    FILE *arch;
    t_socio actual, nuevo, aux;
    t_contexto ctx;
    t_reg_indice reg_ind;
    char tipo_error[TAM_TIPO_ERROR];
    char linea[128];
    char dni_txt[32];
    long dni, clave;
    unsigned nro_reg;
    int ret = ERROR;

    arch = fopen(path, "r+b");
    if (!arch)
    {
        printf("Error: no se pudo abrir el archivo de socios.\n");
        return ERROR;
    }

    printf("\n--- MODIFICACION MANUAL DE SOCIO ---\n");
    printf("DNI del socio a modificar: ");
    fgets(linea, sizeof(linea), stdin);
    if (sscanf(linea, "%ld", &dni) != 1)
    {
        printf("Error en el formato del DNI.\n");
        goto fin;
    }

    memset(&aux, 0, sizeof(t_socio));
    aux.dni = dni;
    clave = clave_socio(&aux);

    reg_ind.id = clave;
    reg_ind.nro_reg = 0;
    if (indice_buscar_ALU(indice, &reg_ind, sizeof(t_reg_indice),
                          compara_reg_indice_ALU) == NO_EXISTE)
    {
        printf("No existe un socio con DNI %ld.\n", dni);
        goto fin;
    }
    nro_reg = reg_ind.nro_reg;

    if (fseek(arch, (long)nro_reg * (long)sizeof(t_socio), SEEK_SET) != 0 ||
        fread(&actual, sizeof(t_socio), 1, arch) != 1)
    {
        printf("Error al leer el socio del archivo.\n");
        goto fin;
    }

    if (actual.estado != 'A')
    {
        printf("El socio esta dado de baja y no se puede modificar.\n");
        goto fin;
    }

    printf("\nDatos actuales:\n");
    socio_mostrar(&actual);
    printf("\nIngrese los nuevos datos (Enter conserva el valor actual)\n");

    nuevo = actual;

    printf("CUIL [%s]: ", actual.cuil);
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    if (linea[0] != '\0')
    {
        strncpy(nuevo.cuil, linea, sizeof(nuevo.cuil) - 1);
        nuevo.cuil[sizeof(nuevo.cuil) - 1] = '\0';
    }

    printf("Apellidos y nombres [%s]: ", actual.apellidos_nombres);
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    if (linea[0] != '\0')
    {
        strncpy(nuevo.apellidos_nombres, linea, sizeof(nuevo.apellidos_nombres) - 1);
        nuevo.apellidos_nombres[sizeof(nuevo.apellidos_nombres) - 1] = '\0';
    }

    printf("Fecha de nacimiento [%02d/%02d/%04d]: ", actual.fecha_nacimiento.dia,
           actual.fecha_nacimiento.mes, actual.fecha_nacimiento.anio);
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    if (linea[0] != '\0' &&
        sscanf(linea, "%d/%d/%d", &nuevo.fecha_nacimiento.dia,
               &nuevo.fecha_nacimiento.mes, &nuevo.fecha_nacimiento.anio) != 3)
    {
        printf("Error en el formato de la fecha de nacimiento.\n");
        goto fin;
    }

    printf("Sexo (H/M) [%c]: ", actual.sexo);
    fgets(linea, sizeof(linea), stdin);
    if (linea[0] != '\n' && linea[0] != '\0')
        nuevo.sexo = (char)toupper((unsigned char)linea[0]);

    printf("Fecha de afiliacion [%02d/%02d/%04d]: ", actual.fecha_afiliacion.dia,
           actual.fecha_afiliacion.mes, actual.fecha_afiliacion.anio);
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    if (linea[0] != '\0' &&
        sscanf(linea, "%d/%d/%d", &nuevo.fecha_afiliacion.dia,
               &nuevo.fecha_afiliacion.mes, &nuevo.fecha_afiliacion.anio) != 3)
    {
        printf("Error en el formato de la fecha de afiliacion.\n");
        goto fin;
    }

    printf("Categoria (%s/%s) [%s]: ", CAT_MENOR, CAT_ADULTO, actual.categoria);
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    if (linea[0] != '\0')
    {
        strncpy(nuevo.categoria, linea, sizeof(nuevo.categoria) - 1);
        nuevo.categoria[sizeof(nuevo.categoria) - 1] = '\0';
    }

    printf("Fecha de ultima cuota [%02d/%02d/%04d]: ", actual.fecha_ultima_cuota.dia,
           actual.fecha_ultima_cuota.mes, actual.fecha_ultima_cuota.anio);
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    if (linea[0] != '\0' &&
        sscanf(linea, "%d/%d/%d", &nuevo.fecha_ultima_cuota.dia,
               &nuevo.fecha_ultima_cuota.mes, &nuevo.fecha_ultima_cuota.anio) != 3)
    {
        printf("Error en el formato de la fecha de ultima cuota.\n");
        goto fin;
    }

    printf("Plan (BASIC/PREMIUM/VIP/FAMILY) [%s]: ", actual.plan);
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    if (linea[0] != '\0')
    {
        strncpy(nuevo.plan, linea, sizeof(nuevo.plan) - 1);
        nuevo.plan[sizeof(nuevo.plan) - 1] = '\0';

    }

    printf("Email tutor ('-' para borrar) [%s]: ", actual.email_tutor);
    fgets(linea, sizeof(linea), stdin);
    linea[strcspn(linea, "\n")] = '\0';
    if (strcmp(linea, "-") == 0)
        nuevo.email_tutor[0] = '\0';
    else if (linea[0] != '\0')
    {
        strncpy(nuevo.email_tutor, linea, sizeof(nuevo.email_tutor) - 1);
        nuevo.email_tutor[sizeof(nuevo.email_tutor) - 1] = '\0';
    }

    ctx.indice = indice;
    ctx.arch_bin = arch;
    ctx.fecha_proceso = fecha_proceso;
    ctx.id_modificando = clave;

    tipo_error[0] = '\0';
    if (valida_socio_ALU(&nuevo, &ctx, tipo_error) == OK)
    {
        if (fseek(arch, (long)nro_reg * (long)sizeof(t_socio), SEEK_SET) != 0 ||
            fwrite(&nuevo, sizeof(t_socio), 1, arch) != 1)
        {
            printf("Error al escribir el socio.\n");
            goto fin;
        }
        fflush(arch);
        printf("\nSocio modificado exitosamente.\n");
        ret = OK;
    }
    else
    {
        sprintf(dni_txt, "%ld", nuevo.dni);   /* la auditoria guarda el DNI */
        rechaza_socio(&nuevo, dni_txt, tipo_error, fecha_proceso);
        printf("\nModificacion RECHAZADA. Validacion fallida: %s\n", tipo_error);
    }

fin:
    fclose(arch);
    return ret;
}

/******************************************************************************
Descripcion:    baja logica de socio: busca el DNI en el indice, pone estado
                'B' en el registro del archivo y lo elimina del indice.
Parametros:     indice: indice de socios.
                path: ruta de socios.dat.
Retorno:        OK si se dio de baja, ERROR si no existe o ya estaba dado de
                baja.
Observaciones:  -
******************************************************************************/
int socio_baja_ALU(t_indice *indice, const char *path)
{
    t_reg_indice RegBusqueda;
    int pos;
    char linea[32];
    long dniB;
    char* fin;
    t_socio socio;

    FILE* FP = fopen(path, "r+b");
    if(!FP)
    {
        printf("Error al abrir el archivo.\n");
        return ERROR;
    }

    printf("DNI del socio a dar de baja: ");
    if(!fgets(linea, sizeof(linea), stdin))
    {
        fclose(FP);
        return ERROR;
    }


    dniB = strtol(linea, &fin, 10);  /*strtol lo que hace es basicamente pasar de una cadena de caracteres a un valor entero de tipo long.
                                      fin lo que tiene es la direccion del ultimo lugar donde no pudo leer, ej: en "1234\n", tendria la direccion del \n.
                                      por ultimo, el 10 significa basicamente la base en la cual lee, base 10 es la de los digitos normales, si se pone base 0 adivina la base, pero
                                      nosotros ya conocemos la base entonces simplemente le pongo la base que corresponde y listo. */

    if(fin == linea || (*fin != '\n' && *fin != '\0'))
    {
        /*si fin es igual a linea, entonces eso significa que literalmente no habia ningun digito. (es decir, enter o letra o nada.)
                                                               ahora, si fin es distinto de \n o \0 significa que hay basura dps del numero, ej: "123abc" */
        printf("DNI invalido.\n");
        fclose(FP);
        return ERROR;
    }

    RegBusqueda.id = dniB;
    RegBusqueda.nro_reg = 0;
    pos = indice_buscar_ALU(indice, &RegBusqueda, sizeof(t_reg_indice), compara_reg_indice_ALU);

    if(pos == NO_EXISTE)
    {
        printf("El socio no existe o ya esta dado de baja.\n");
        fclose(FP);
        return ERROR;
    }
    if(socio_leer_ALU(FP, RegBusqueda.nro_reg, &socio)!=OK)
    {
        fclose(FP);
        return ERROR;
    }

    socio.estado='B';

    if(socio_escribir_ALU(FP, RegBusqueda.nro_reg, &socio)!=OK)
    {
        fclose(FP);
        return ERROR;
    }
    fclose(FP);

    if(indice_eliminar_ALU(indice, &RegBusqueda, sizeof(t_reg_indice), compara_reg_indice_ALU)!=OK)
    {
        return ERROR;
    }

    printf("Socio %ld (%s) dado de baja.\n", socio.dni, socio.apellidos_nombres);

    return OK;
}

/******************************************************************************
Descripcion:    recorre el indice (ordenado por DNI y solo con activos) y
                lista los socios leyendo cada registro del archivo.
Parametros:     indice: indice de socios.
                path: ruta de socios.dat.
Retorno:        OK si se pudo abrir el archivo, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socios_listar_ALU(const t_indice *indice, const char *path)
{
    t_reg_indice *Aux = (t_reg_indice*) indice->vindice;
    t_socio Socio;
    FILE* PF = fopen(path, "rb");

    if(!PF) return ERROR;

    for(unsigned i=0 ; i < indice->cantidad_elementos_actual ; i++)
    {
        if(socio_leer_ALU(PF, Aux->nro_reg, &Socio))
        {
            socio_mostrar(&Socio);
        }
        Aux++;
    }

    fclose(PF);
    return OK;
}

/******************************************************************************
Descripcion:    recorre el indice y exporta a un archivo binario los socios
                con plan PREMIUM o VIP y cuota al dia (el estado 'A' lo
                garantiza el indice). Sobrescribe el archivo de exportacion
                anterior.
Parametros:     indice: indice de socios.
                path: ruta de socios.dat.
                path_export: ruta de beta_testers.dat.
                fecha_proceso: fecha de proceso.
Retorno:        OK si se pudo exportar, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socios_exportar_ALU(const t_indice *indice, const char *path, const char *path_export,
                        const t_fecha *fecha_proceso)
{
    const t_reg_indice* RegAux = indice->vindice;
    t_socio s;
    FILE* AS;
    FILE* ABT;
    int CantSocios=0;

    AS = fopen(path, "rb");
    if(!AS)
        return ERROR;

    ABT = fopen(path_export, "wb");
    if(!ABT)
    {
        fclose(AS);
        return ERROR;
    }

    for(unsigned int i=0 ; i < indice->cantidad_elementos_actual ; i++)
    {
        if(socio_leer_ALU(AS, RegAux->nro_reg, &s)!= OK)            //leo el socio y lo guardo en una variable auxiliar (en este caso en s)
        {
            fclose(AS);
            fclose(ABT);
            return ERROR;
        }

        if( (!strcmp(s.plan, "VIP")|| !strcmp(s.plan, "PREMIUM"))  && (cuota_al_dia_ALU(&s.fecha_ultima_cuota, fecha_proceso)== 1) )  //como ya tengo el socio, hago las comparaciones necesarias
        {
            if(fwrite(&s, sizeof(t_socio), 1, ABT)!=1)  //escribo en el archivo, y en caso de que falle cierro los archivos y devuelvo ERROR.
            {
                fclose(AS);
                fclose(ABT);
                return ERROR;
            }
            CantSocios++; //aumento la cantidad de socios para que de esta forma saber cuantos exporte.
        }

        RegAux++;
    }

    printf("Socios exportados a %s: %d\n", path_export,CantSocios);

    fclose(AS);
    fclose(ABT);
    return OK;
}

/******************************************************************************
Descripcion:    opcion 2 del menu: submenu para elegir entre alta y
                modificacion manual de socio.
Parametros:     ind_socios: indice de socios.
                fecha_proceso: fecha de proceso.
Retorno:        No posee.
Observaciones:  -
******************************************************************************/
void abm_socio_ALU(t_indice *indice, const t_fecha *fecha_proceso)
{
    int opcion, c;
    const char *path_socios = ARCH_SOCIOS_BIN;

    do
    {
        printf("\n--- Alta / Modificacion de socio ---\n");
        printf("1. Alta manual de socio\n");
        printf("2. Modificacion manual de socio\n");
        printf("0. Volver al menu principal\n");
        printf("Ingrese una opcion: ");

        if (scanf("%d", &opcion) != 1)
            opcion = -1;
        while ((c = getchar()) != '\n' && c != EOF)   /* limpia el resto de la linea */
            ;
        switch (opcion)
        {
        case 1:
            socio_alta_manual_ALU(indice, path_socios, fecha_proceso);
            break;
        case 2:
            socio_modificacion_manual_ALU(indice, path_socios, fecha_proceso);
            break;
        case 0:
            break;
        default:
            printf("Opcion invalida. Intente nuevamente.\n");
        }
    }
    while (opcion != 0);
}
