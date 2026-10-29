#include <string.h>
#include "lectura.h"
#include "util.h"
#include "cadena.h"
#include "socio.h"
#include "titulo.h"
#include "config.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    funcion generica de lectura de archivos de texto. Por cada
                linea no vacia: parsear -> validar -> escribir al final del
                archivo binario e insertar en el indice. Ante el primer error
                (de formato o de validacion) se rechaza la linea invocando
                rechazar y se continua con la siguiente.
Parametros:     path_txt: ruta del archivo de texto (CSV).
                arch_bin: archivo binario destino abierto en "w+b" o "r+b".
                indice: indice de la entidad (se usa para validar duplicados y
                se va completando).
                reg: buffer de un registro de la entidad.
                tamanyo: tamanyo en bytes del registro.
                fecha_proceso: fecha de proceso.
                parsear: convierte una linea en registro.
                validar: valida el registro y deja el tipo de error.
                rechazar: accion ante un rechazo (auditar, ignorar, etc.).
                obtener_clave: devuelve la clave del registro para el indice.
                aceptados, rechazados: salida; contadores de registros.
Retorno:        OK si se pudo abrir el archivo de texto, ERROR en caso
                contrario.
Observaciones:  Se reutiliza para Socios.csv y Titulos.csv; lo especifico de
                cada entidad lo resuelven los punteros a funcion.
******************************************************************************/
int leer_archivo_texto_ALU(const char *path_txt, FILE *arch_bin, t_indice *indice,
                           void *reg, size_t tamanyo, const t_fecha *fecha_proceso,
                           int (*parsear)(const char *, void *),
                           int (*validar)(void *, const t_contexto *, char *),
                           void (*rechazar)(const void *, const char *, const char *, const t_fecha *),
                           long (*obtener_clave)(const void *),
                           unsigned *aceptados, unsigned *rechazados)
{
    char linea [TAM_LINEA], tipo_error[TAM_TIPO_ERROR];
    int resultado,error;
    long prox;
    t_contexto contexto;
    t_reg_indice nuevo_reg;
    *aceptados=0;
    *rechazados=0;

    FILE *arch=fopen(path_txt,"rt");
    if(!arch)
    {
        printf("No se pudo abrir el archivo %s\n", path_txt);
        return ERROR;
    }

    contexto.indice=indice;
    contexto.arch_bin=arch_bin;
    contexto.fecha_proceso=fecha_proceso;
    contexto.id_modificando=0;

    while(fgets(linea,TAM_LINEA,arch) != NULL)
    {
        quitar_salto(linea);
        if(cadena_vacia(linea))
        {
            continue;
        }
        resultado=parsear(linea,reg);

        if(resultado == ERROR)
        {
            (*rechazados) ++;

            rechazar(reg,linea,"FORMATO",fecha_proceso);
            continue;
        }

        error=validar(reg,&contexto,tipo_error);

        if(error == ERROR)
        {
            (*rechazados) ++;
            rechazar(reg,linea,tipo_error,fecha_proceso);
            continue;
        }

        fseek(arch_bin,0,SEEK_END);
        prox=ftell(arch_bin)/tamanyo;


        if(fwrite(reg,tamanyo,1,arch_bin) != 1)
        {
            printf("No se pudo grabar en el archivo binario\n");
            fclose(arch);
            return ERROR;
        }
        fflush(arch_bin);


        nuevo_reg.id = obtener_clave(reg);
        nuevo_reg.nro_reg=prox;

        if(indice_insertar_ALU(indice, &nuevo_reg, sizeof(t_reg_indice), compara_reg_indice_ALU) != OK)
        {
            printf("No se pudo insertar en el indice\n");
            fclose(arch);
            return ERROR;
        }

        (*aceptados)++;
    }

    fclose(arch);

    return OK;
}

/******************************************************************************
Descripcion:    compara dos t_reg_indice por su clave id.
Parametros:     a, b: punteros a t_reg_indice.
Retorno:        Negativo si a < b, 0 si son iguales, positivo si a > b.
Observaciones:  Se usa como cmp en todas las operaciones del TDA indice.
******************************************************************************/
int compara_reg_indice_ALU(const void *a, const void *b)
{
    const t_reg_indice *r1 = (const t_reg_indice*)a;
    const t_reg_indice *r2 = (const t_reg_indice*)b;

    if(r1->id < r2->id)
        return -1;

    if(r1->id > r2->id)
        return 1;
    return 0;
}

/******************************************************************************
Descripcion:    construye los indices de socios y de titulos a partir de los
                archivos binarios existentes (solo registros activos).
Parametros:     ind_socios: indice de socios.
                ind_titulos: indice de titulos.
Retorno:        1 si ambos binarios existen y se cargaron, 0 en caso
                contrario.
Observaciones:  -
******************************************************************************/
int cargar_indices_ALU(t_indice *ind_socios, t_indice *ind_titulos)
{
    int return_socio=indice_cargar_ALU(ARCH_SOCIOS_BIN,ind_socios,sizeof(t_socio),clave_socio,activo_socio,compara_reg_indice_ALU);
    int return_titulo=indice_cargar_ALU(ARCH_TITULOS_BIN,ind_titulos,sizeof(t_titulo),clave_titulo,activo_titulo,compara_reg_indice_ALU);

    if(return_socio== OK && return_titulo== OK)
    {
        return 1;
    }
    return 0;

}

/******************************************************************************
Descripcion:    opcion 1 del menu: lee Socios.csv y Titulos.csv con la funcion
                generica de lectura, genera socios.dat y titulos.dat
                (sobrescribiendolos) y reconstruye los indices desde los
                binarios generados.
Parametros:     ind_socios: indice de socios.
                ind_titulos: indice de titulos.
                fecha_proceso: fecha de proceso.
Retorno:        1 si los indices quedaron cargados, 0 en caso contrario.
Observaciones:  Los rechazos de socios se auditan en errores_socios.txt; los
                de titulos solo se cuentan.
******************************************************************************/
int generar_binarios_ALU(t_indice *ind_socios, t_indice *ind_titulos, const t_fecha *fecha_proceso)
{
    FILE *fsocio_bin;
    FILE *ftitulo_bin;
    t_socio s;
    t_titulo t;
    unsigned aceptados_s=0,rechazados_s=0;
    unsigned aceptados_t=0,rechazados_t=0;
    int resultado_socio, resultado_titulo, resultado_carga;

    indice_vaciar_ALU(ind_socios);
    indice_vaciar_ALU(ind_titulos);

    fsocio_bin = fopen(ARCH_SOCIOS_BIN, "w+b");
    if(fsocio_bin == NULL)
    {
        printf("No se pudo crear %s\n", ARCH_SOCIOS_BIN);
        return 0;
    }
    ftitulo_bin= fopen(ARCH_TITULOS_BIN, "w+b");

    if(ftitulo_bin == NULL)
    {
        printf("No se pudo crear %s\n", ARCH_TITULOS_BIN);
        fclose(fsocio_bin);
        return 0;
    }

    resultado_socio=leer_archivo_texto_ALU(ARCH_SOCIOS_CSV,fsocio_bin,ind_socios,&s,sizeof(t_socio),fecha_proceso,texto_a_socio_ALU,valida_socio_ALU,rechaza_socio,clave_socio,&aceptados_s,&rechazados_s);
    resultado_titulo=leer_archivo_texto_ALU(ARCH_TITULOS_CSV,ftitulo_bin,ind_titulos,&t,sizeof(t_titulo),fecha_proceso,texto_a_titulo_ALU,valida_titulo_ALU,rechaza_titulo,clave_titulo,&aceptados_t,&rechazados_t);


    fclose(ftitulo_bin);
    fclose(fsocio_bin);

    if (resultado_socio == ERROR || resultado_titulo== ERROR)
    {
        printf("No se pudieron leer los archivos de texto\n");
        return 0;
    }

    printf("Socios:  %u aceptados, %u rechazados (ver %s)\n", aceptados_s, rechazados_s, ARCH_ERRORES);
    printf("Titulos: %u aceptados, %u rechazados\n", aceptados_t, rechazados_t);

    resultado_carga = cargar_indices_ALU(ind_socios,ind_titulos);

    return resultado_carga;
}
