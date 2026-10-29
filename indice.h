#ifndef INDICE_H_INCLUDED
#define INDICE_H_INCLUDED

#include <stddef.h>

#define CANTIDAD_ELEMENTOS 100
#define INCREMENTO 1.3
#define OK 1
#define ERROR 0
#define NO_EXISTE -1

typedef struct
{
    unsigned nro_reg;
    long     id;
} t_reg_indice;

typedef struct
{
    void     *vindice;
    unsigned  cantidad_elementos_actual;
    unsigned  cantidad_elementos_maxima;
} t_indice;

/**************************************************************************
Descripcion:    toma memoria para nmemb elementos e inicializa la
                estructura vacia.
Parametros:     indice: TDA indice.
                nmemb: cantidad inicial de elementos del indice
                       (CANTIDAD_ELEMENTOS).
                tamanyo: espacio en bytes ocupado por cada elemento.
Retorno:        OK si la operacion fue exitosa y ERROR en caso contrario.
Observaciones:  -
**************************************************************************/
int indice_crear(t_indice *indice, size_t nmemb, size_t tamanyo);

/**************************************************************************
Descripcion:    redimensiona el arreglo interno del indice.
Parametros:     indice: TDA indice.
                tamanyo: espacio en bytes ocupado por cada elemento.
Retorno:        OK si la operacion fue exitosa y ERROR en caso contrario.
Observaciones:  La nueva capacidad es la capacidad actual multiplicada
                por INCREMENTO (30 % mas). La calcula la propia funcion.
**************************************************************************/
int indice_redimensionar(t_indice *indice, size_t tamanyo);

/**************************************************************************
Descripcion:    inserta un elemento manteniendo el orden segun la clave.
Parametros:     indice: TDA indice.
                registro: elemento a insertar.
                tamanyo: espacio en bytes ocupado por el elemento.
                cmp: funcion de comparacion provista.
Retorno:        OK si la operacion fue exitosa y ERROR en caso contrario
                (por ejemplo, clave duplicada o falta de memoria).
Observaciones:  Si el arreglo esta lleno, invoca indice_redimensionar.
**************************************************************************/
int indice_insertar(t_indice *indice, const void *registro, size_t tamanyo,
                    int (*cmp)(const void *, const void *));

/**************************************************************************
Descripcion:    elimina el elemento cuya clave coincide con la de registro.
Parametros:     indice: TDA indice.
                registro: elemento a eliminar.
                tamanyo: espacio en bytes ocupado por el elemento.
                cmp: funcion de comparacion provista.
Retorno:        OK si la operacion fue exitosa y ERROR en caso contrario.
Observaciones:  -
**************************************************************************/
int indice_eliminar(t_indice *indice, const void *registro, size_t tamanyo,
                    int (*cmp)(const void *, const void *));

/**************************************************************************
Descripcion:    busca la clave de registro en el indice. Si existe, copia
                el elemento completo en registro.
Parametros:     indice: TDA indice.
                registro: elemento a buscar (entrada/salida).
                tamanyo: espacio en bytes ocupado por el elemento.
                cmp: funcion de comparacion provista.
Retorno:        NO_EXISTE si la clave no existe; en caso contrario, la
                posicion que ocupa dentro del arreglo.
Observaciones:  La comparacion entre claves se realiza exclusivamente
                mediante la funcion cmp recibida por puntero.
**************************************************************************/
int indice_buscar(const t_indice *indice, void *registro, size_t tamanyo,
                  int (*cmp)(const void *, const void *));

/**************************************************************************
Descripcion:    determina si el indice contiene 0 (cero) elementos.
Parametros:     indice: TDA indice.
Retorno:        OK si esta vacio, cualquier otro valor si no lo esta.
Observaciones:  -
**************************************************************************/
int indice_vacio(const t_indice *indice);

/**************************************************************************
Descripcion:    determina si el indice alcanzo su capacidad maxima actual.
Parametros:     indice: TDA indice.
Retorno:        OK si esta lleno, cualquier otro valor si no lo esta.
Observaciones:  -
**************************************************************************/
int indice_lleno(const t_indice *indice);

/**************************************************************************
Descripcion:    deja el indice vacio sin liberar la memoria del arreglo.
Parametros:     indice: TDA indice.
Retorno:        No posee.
Observaciones:  -
**************************************************************************/
void indice_vaciar(t_indice *indice);

/**************************************************************************
Descripcion:    libera toda la memoria dinamica del indice.
Parametros:     indice: TDA indice.
Retorno:        No posee.
Observaciones:  Luego de invocarla el indice no puede usarse sin volver
                a llamar a indice_crear.
**************************************************************************/
void indice_destruir(t_indice *indice);

/**************************************************************************
Descripcion:    construye el indice a partir de un archivo binario de
                registros. Recorre el archivo secuencialmente y, por cada
                registro leido que este activo, inserta en el indice un
                t_reg_indice (nro_reg = posicion del registro en el
                archivo, id = clave obtenida mediante obtener_clave).
Parametros:     path: ruta al archivo binario.
                indice: TDA indice.
                tamanyo_reg: tamano en bytes de cada registro del archivo.
                obtener_clave: funcion que, dado un registro del archivo,
                               devuelve su clave (long).
                es_activo: funcion que, dado un registro del archivo,
                           devuelve distinto de 0 si su estado es 'A'.
                cmp: funcion de comparacion provista.
Retorno:        OK si la operacion fue exitosa y ERROR en caso contrario.
Observaciones:  Los registros con estado 'B' se omiten. El archivo no
                necesita estar ordenado: el orden lo garantiza
                indice_insertar.
**************************************************************************/
int indice_cargar(const char *path, t_indice *indice, size_t tamanyo_reg,
                  long (*obtener_clave)(const void *),
                  int (*es_activo)(const void *),
                  int (*cmp)(const void *, const void *));

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

int indice_crear_ALU(t_indice *indice, size_t nmemb, size_t tamanyo);
int indice_redimensionar_ALU(t_indice *indice, size_t tamanyo);
int indice_insertar_ALU(t_indice *indice, const void *registro, size_t tamanyo,
                    int (*cmp)(const void *, const void *));
int indice_eliminar_ALU(t_indice *indice, const void *registro, size_t tamanyo,
                    int (*cmp)(const void *, const void *));
int indice_buscar_ALU(const t_indice *indice, void *registro, size_t tamanyo,
                  int (*cmp)(const void *, const void *));
int indice_vacio_ALU(const t_indice *indice);
int indice_lleno_ALU(const t_indice *indice);
void indice_vaciar_ALU(t_indice *indice);
void indice_destruir_ALU(t_indice *indice);
int indice_cargar_ALU(const char *path, t_indice *indice, size_t tamanyo_reg,
                  long (*obtener_clave)(const void *),
                  int (*es_activo)(const void *),
                  int (*cmp)(const void *, const void *));

#endif // INDICE_H_INCLUDED
