#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "indice.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    toma memoria para nmemb elementos e inicializa la estructura
                vacia.
Parametros:     indice: TDA indice.
                nmemb: cantidad inicial de elementos (CANTIDAD_ELEMENTOS).
                tamanyo: espacio en bytes ocupado por cada elemento.
Retorno:        OK si la operacion fue exitosa y ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int indice_crear_ALU(t_indice *indice, size_t nmemb, size_t tamanyo)
{
    indice->vindice = malloc(nmemb * tamanyo);
    if(indice->vindice == NULL)
    {
        return ERROR;
    }

    indice->cantidad_elementos_actual = 0;
    indice->cantidad_elementos_maxima = nmemb;

    return OK;
}

/******************************************************************************
Descripcion:    redimensiona el arreglo interno del indice con realloc. La
                nueva capacidad es la actual multiplicada por INCREMENTO (30%
                mas); la calcula la propia funcion.
Parametros:     indice: TDA indice.
                tamanyo: espacio en bytes ocupado por cada elemento.
Retorno:        OK si la operacion fue exitosa y ERROR en caso contrario.
Observaciones:  Si el producto no aumenta la capacidad (arreglos muy chicos)
                se garantiza al menos un elemento mas.
******************************************************************************/
int indice_redimensionar_ALU(t_indice *indice, size_t tamanyo)
{
    size_t nueva_cap = indice->cantidad_elementos_maxima * INCREMENTO;
    void *aux;

    if(nueva_cap <= indice->cantidad_elementos_maxima)
        nueva_cap = indice->cantidad_elementos_maxima + 1;

    aux = realloc(indice->vindice, nueva_cap * tamanyo);
    if(!aux) return ERROR;

    indice->vindice = aux;
    indice->cantidad_elementos_maxima = nueva_cap;
    return OK;
}

/******************************************************************************
Descripcion:    inserta un elemento manteniendo el orden segun la clave. Si el
                arreglo esta lleno invoca indice_redimensionar.
Parametros:     indice: TDA indice.
                registro: elemento a insertar.
                tamanyo: espacio en bytes ocupado por el elemento.
                cmp: funcion de comparacion provista.
Retorno:        OK si la operacion fue exitosa y ERROR en caso contrario
                (clave duplicada o falta de memoria).
Observaciones:  -
******************************************************************************/
int indice_insertar_ALU(t_indice *indice, const void *registro, size_t tamanyo,
                    int (*cmp)(const void *, const void *))
{
    char *base;
    int   ini = 0;
    int   fin = (int)indice->cantidad_elementos_actual - 1;
    int   medio;
    int   res;

    /* busqueda binaria de la posicion donde va el registro */
    while(ini <= fin)
    {
        medio = ini + (fin - ini) / 2;
        res = cmp(registro, (char *)indice->vindice + medio * tamanyo);

        if(res == 0)
            return ERROR;                       /* clave duplicada */
        if(res < 0)
            fin = medio - 1;
        else
            ini = medio + 1;
    }
    /* ini quedo en la posicion donde hay que insertar */

    if(indice_lleno_ALU(indice) == OK && indice_redimensionar_ALU(indice, tamanyo) != OK)
        return ERROR;                           /* sin memoria */

    base = (char *)indice->vindice;             /* despues del realloc */
    memmove(base + (ini + 1) * tamanyo, base + ini * tamanyo,
            (indice->cantidad_elementos_actual - ini) * tamanyo);
    memcpy(base + ini * tamanyo, registro, tamanyo);
    indice->cantidad_elementos_actual++;

    return OK;
}

/******************************************************************************
Descripcion:    elimina el elemento cuya clave coincide con la de registro,
                desplazando los siguientes.
Parametros:     indice: TDA indice.
                registro: elemento a eliminar (solo importa su clave).
                tamanyo: espacio en bytes ocupado por el elemento.
                cmp: funcion de comparacion provista.
Retorno:        OK si la operacion fue exitosa y ERROR si la clave no existe.
Observaciones:  -
******************************************************************************/
int indice_eliminar_ALU(t_indice *indice, const void *registro, size_t tamanyo,
                        int (*cmp)(const void *, const void *))
{
    char* AuxIndice = (char *)indice->vindice;
    void* reg_aux;
    int pos;

    reg_aux = malloc(tamanyo);   /* copia porque buscar escribe en el registro */
    if(!reg_aux)
        return ERROR;

    memcpy(reg_aux, registro, tamanyo);
    pos = indice_buscar_ALU(indice, reg_aux, tamanyo, cmp);
    free(reg_aux);

    if(pos == NO_EXISTE)
        return ERROR;

    memmove(AuxIndice + pos * tamanyo, AuxIndice + (pos + 1) * tamanyo, (indice->cantidad_elementos_actual - pos - 1) * tamanyo);

    indice->cantidad_elementos_actual--;
    return OK;
}

/******************************************************************************
Descripcion:    busca la clave de registro en el indice. Si existe copia el
                elemento completo (nro_reg incluido) en registro.
Parametros:     indice: TDA indice.
                registro: elemento a buscar (entrada/salida).
                tamanyo: espacio en bytes ocupado por el elemento.
                cmp: funcion de comparacion provista.
Retorno:        NO_EXISTE si la clave no existe; en caso contrario la posicion
                que ocupa dentro del arreglo.
Observaciones:  La comparacion entre claves se realiza exclusivamente mediante
                cmp.
******************************************************************************/
int indice_buscar_ALU(const t_indice *indice, void *registro, size_t tamanyo,
                  int (*cmp)(const void *, const void *))
{
    int ini = 0,fin = indice->cantidad_elementos_actual -1, medio;

    while(ini <= fin)
    {
        medio = (ini + fin)/2;
        void * elem_medio = (char *)indice->vindice + medio * tamanyo;
        int comp = cmp(registro,elem_medio);

        if(comp == 0)
        {
            memcpy(registro, elem_medio, tamanyo);   /* devuelve el nro_reg */
            return medio;
        }
        else if(comp < 0) fin = medio -1;
        else ini = medio + 1;
    }
    return NO_EXISTE;
}

/******************************************************************************
Descripcion:    determina si el indice contiene 0 (cero) elementos.
Parametros:     indice: TDA indice.
Retorno:        OK si esta vacio, ERROR si no lo esta.
Observaciones:  -
******************************************************************************/
int indice_vacio_ALU(const t_indice *indice)
{
    return indice->cantidad_elementos_actual==0;
}

/******************************************************************************
Descripcion:    determina si el indice alcanzo su capacidad maxima actual.
Parametros:     indice: TDA indice.
Retorno:        OK si esta lleno, ERROR si no lo esta.
Observaciones:  -
******************************************************************************/
int indice_lleno_ALU(const t_indice *indice)
{
    return indice->cantidad_elementos_actual==  indice->cantidad_elementos_maxima;

}

/******************************************************************************
Descripcion:    deja el indice vacio sin liberar la memoria del arreglo.
Parametros:     indice: TDA indice.
Retorno:        No posee.
Observaciones:  -
******************************************************************************/
void indice_vaciar_ALU(t_indice *indice)
{
    indice->cantidad_elementos_actual= 0;
}

/******************************************************************************
Descripcion:    libera toda la memoria dinamica del indice.
Parametros:     indice: TDA indice.
Retorno:        No posee.
Observaciones:  Luego de invocarla el indice no puede usarse sin volver a
                llamar a indice_crear.
******************************************************************************/
void indice_destruir_ALU(t_indice *indice)
{
    free(indice->vindice);
    indice->vindice = NULL;
    indice->cantidad_elementos_actual = 0;
    indice->cantidad_elementos_maxima = 0;
}

/******************************************************************************
Descripcion:    construye el indice a partir de un archivo binario de
                registros. Recorre el archivo secuencialmente y por cada
                registro activo inserta un t_reg_indice (nro_reg = posicion en
                el archivo, id = clave obtenida con obtener_clave).
Parametros:     path: ruta al archivo binario.
                indice: TDA indice (ya creado; se vacia antes de cargar).
                tamanyo_reg: tamanyo en bytes de cada registro del archivo.
                obtener_clave: funcion que devuelve la clave (long) de un
                registro.
                es_activo: funcion que devuelve distinto de 0 si el registro
                tiene estado 'A'.
                cmp: funcion de comparacion provista.
Retorno:        OK si la operacion fue exitosa y ERROR en caso contrario.
Observaciones:  Los registros con estado 'B' se omiten. El archivo no necesita
                estar ordenado: el orden lo garantiza indice_insertar.
******************************************************************************/
int indice_cargar_ALU(const char *path, t_indice *indice, size_t tamanyo_reg,
                  long (*obtener_clave)(const void *),
                  int (*es_activo)(const void *),
                  int (*cmp)(const void *, const void *))
{
    FILE* arch;
    void* reg;
    t_reg_indice reg_indice;
    unsigned nro_reg = 0;

    arch = fopen(path, "rb");
    if(!arch)
        return ERROR;

    reg = malloc(tamanyo_reg);
    if(!reg){
        fclose(arch);
        return ERROR;
    }

    indice_vaciar_ALU(indice);

    while(fread(reg, tamanyo_reg, 1, arch) == 1){
            if(es_activo(reg)){
                reg_indice.nro_reg = nro_reg;
                reg_indice.id = obtener_clave(reg);

                if(indice_insertar_ALU(indice,&reg_indice, sizeof(t_reg_indice), cmp) != OK){
                    free(reg);
                    fclose(arch);
                    return ERROR;
                }
            }
        nro_reg++;
    }

    free(reg);
    fclose(arch);
    return OK;
}
