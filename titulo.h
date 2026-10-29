#ifndef TITULO_H_INCLUDED
#define TITULO_H_INCLUDED

#include <stdio.h>
#include "indice.h"
#include "lectura.h"

#define TAM_TITULO    61
#define TAM_GENERO    21
#define CAMPOS_TITULO 4
#define CANT_GENEROS  5

typedef struct
{
    int  id;
    char titulo[TAM_TITULO];
    char genero[TAM_GENERO];
    int  stock;
    char estado;
} t_titulo;

extern const char *const GENEROS[CANT_GENEROS];

/* funciones para la lectura generica */
int  texto_a_titulo(const char *linea, void *reg);
int  valida_titulo(void *reg, const t_contexto *ctx, char *tipo_error);
void rechaza_titulo(const void *reg, const char *linea, const char *tipo_error,
                    const t_fecha *fecha_proceso);
long clave_titulo(const void *reg);
int  activo_titulo(const void *reg);

/* acceso al archivo binario */
int  titulo_leer(FILE *arch, unsigned nro_reg, t_titulo *t);
int  titulo_escribir(FILE *arch, unsigned nro_reg, const t_titulo *t);

/* utilidades */
int  genero_indice(const char *genero);
void titulo_mostrar(const t_titulo *t);
void titulos_encabezado(void);
int  titulos_listar(const t_indice *indice, const char *path);

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

int texto_a_titulo_ALU(const char *linea, void *reg);
int valida_titulo_ALU(void *reg, const t_contexto *ctx, char *tipo_error);
int titulos_listar_ALU(const t_indice *indice, const char *path);

#endif /* TITULO_H_INCLUDED */
