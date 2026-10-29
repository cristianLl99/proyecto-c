#ifndef LECTURA_H_INCLUDED
#define LECTURA_H_INCLUDED

#include <stdio.h>
#include "indice.h"
#include "fecha.h"

#define TAM_TIPO_ERROR 25

/* Contexto que reciben las funciones de validacion */
typedef struct
{
    t_indice      *indice;          /* indice de la entidad que se valida */
    FILE          *arch_bin;        /* archivo binario de la entidad (r+b / w+b) */
    const t_fecha *fecha_proceso;
    long           id_modificando;  /* 0 en altas; clave del registro en modificaciones */
} t_contexto;

/* Funciones que resuelven lo especifico de cada entidad (se pasan por puntero):
     parsear:       int  f(const char *linea, void *reg)
     validar:       int  f(void *reg, const t_contexto *ctx, char *tipo_error)
     rechazar:      void f(const void *reg, const char *linea, const char *tipo_error,
                           const t_fecha *fecha_proceso)
     obtener_clave: long f(const void *reg)                                   */

int leer_archivo_texto(const char *path_txt, FILE *arch_bin, t_indice *indice,
                       void *reg, size_t tamanyo, const t_fecha *fecha_proceso,
                       int (*parsear)(const char *, void *),
                       int (*validar)(void *, const t_contexto *, char *),
                       void (*rechazar)(const void *, const char *, const char *, const t_fecha *),
                       long (*obtener_clave)(const void *),
                       unsigned *aceptados, unsigned *rechazados);

int compara_reg_indice(const void *a, const void *b);

int cargar_indices(t_indice *ind_socios, t_indice *ind_titulos);
int generar_binarios(t_indice *ind_socios, t_indice *ind_titulos, const t_fecha *fecha_proceso);

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

int leer_archivo_texto_ALU(const char *path_txt, FILE *arch_bin, t_indice *indice,
                       void *reg, size_t tamanyo, const t_fecha *fecha_proceso,
                       int (*parsear)(const char *, void *),
                       int (*validar)(void *, const t_contexto *, char *),
                       void (*rechazar)(const void *, const char *, const char *, const t_fecha *),
                       long (*obtener_clave)(const void *),
                       unsigned *aceptados, unsigned *rechazados);
int compara_reg_indice_ALU(const void *a, const void *b);
int cargar_indices_ALU(t_indice *ind_socios, t_indice *ind_titulos);
int generar_binarios_ALU(t_indice *ind_socios, t_indice *ind_titulos, const t_fecha *fecha_proceso);

#endif /* LECTURA_H_INCLUDED */
