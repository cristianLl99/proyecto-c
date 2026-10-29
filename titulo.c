#include <stdlib.h>
#include <string.h>
#include "titulo.h"
#include "cadena.h"
#include "util.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    convierte una linea CSV de 4 campos en t_titulo. Los campos
                numericos invalidos quedan en valores que fallan la validacion
                (id = 0, stock = -1). El estado se fija en 'A'.
Parametros:     linea: linea CSV.
                reg: puntero a t_titulo destino.
Retorno:        OK si la cantidad de campos es correcta, ERROR en caso
                contrario.
Observaciones:  Se pasa por puntero como parsear a leer_archivo_texto.
******************************************************************************/
int texto_a_titulo_ALU(const char *linea, void *reg)
{
    t_titulo *t = (t_titulo *)reg;
    char      buf[TAM_LINEA];
    char     *campos[CAMPOS_TITULO];

    copiar_cadena(buf, linea, TAM_LINEA);
    quitar_salto(buf);

    if(separar_campos_ALU(buf, campos, CAMPOS_TITULO) != CAMPOS_TITULO)
        return ERROR;

    t->id = es_numero(campos[0]) ? (int)strtol(campos[0], NULL, 10) : 0;
    copiar_cadena(t->titulo, campos[1], TAM_TITULO);
    copiar_cadena(t->genero, campos[2], TAM_GENERO);
    t->stock = es_numero(campos[3]) ? (int)strtol(campos[3], NULL, 10) : -1;
    t->estado = 'A';

    return OK;
}

/******************************************************************************
Descripcion:    aplica las validaciones del punto 6.2 en orden: ID > 0 y no
                existente en el indice, titulo normalizado no vacio, genero
                dentro de GENEROS[] y stock >= 0. Se detiene en el primer
                error.
Parametros:     reg: puntero a t_titulo (se modifica al normalizar el titulo).
                ctx: contexto con indice e id_modificando.
                tipo_error: salida; nombre de la validacion que fallo.
Retorno:        OK si el titulo es valido, ERROR en caso contrario.
Observaciones:  Se pasa por puntero como validar a leer_archivo_texto. Se reutiliza en las altas del
                lote de novedades.
******************************************************************************/
int valida_titulo_ALU(void *reg, const t_contexto *ctx, char *tipo_error)
{
    t_titulo    *t = (t_titulo *)reg;
    t_reg_indice reg_ind;

    /* 1) ID: mayor que 0 y no existente en el indice */
    reg_ind.id = t->id;
    if(t->id <= 0 ||
       (t->id != ctx->id_modificando &&
        indice_buscar_ALU(ctx->indice, &reg_ind, sizeof(t_reg_indice),
                          compara_reg_indice_ALU) != NO_EXISTE))
    {
        strcpy(tipo_error, "ID");
        return ERROR;
    }

    /* 2) Titulo: se normaliza y no puede quedar vacio */
    arreglar_titulo(t->titulo);
    if(cadena_vacia(t->titulo))
    {
        strcpy(tipo_error, "TITULO");
        return ERROR;
    }

    /* 3) Genero: debe ser uno de GENEROS[] */
    if(genero_indice(t->genero) == -1)
    {
        strcpy(tipo_error, "GENERO");
        return ERROR;
    }

    /* 4) Stock: mayor o igual a 0 */
    if(t->stock < 0)
    {
        strcpy(tipo_error, "STOCK");
        return ERROR;
    }

    return OK;
}

/******************************************************************************
Descripcion:    recorre el indice (ordenado por ID y solo con activos) y lista
                los titulos leyendo cada registro del archivo.
Parametros:     indice: indice de titulos.
                path: ruta de titulos.dat.
Retorno:        OK si se pudo abrir el archivo, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int titulos_listar_ALU(const t_indice *indice, const char *path)
{
    FILE         *arch;
    t_titulo      t;
    t_reg_indice *reg_ind = (t_reg_indice *)indice->vindice;
    unsigned      i;

    arch = fopen(path, "rb");
    if(!arch)
    {
        printf("No se pudo abrir %s\n", path);
        return ERROR;
    }

    titulos_encabezado();
    for(i = 0; i < indice->cantidad_elementos_actual; i++)
    {
        if(titulo_leer(arch, reg_ind[i].nro_reg, &t))
            titulo_mostrar(&t);
    }
    printf("\nTotal de titulos activos: %u\n", indice->cantidad_elementos_actual);

    fclose(arch);
    return OK;
}
