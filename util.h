#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "alquiler.h"

#define TAM_LINEA 512

#ifdef _WIN32
#define LIMPIAR_PANTALLA() system("cls")
#define PAUSA()            system("pause")
#else
#define LIMPIAR_PANTALLA() do { if(system("clear")) {} } while(0)
#define PAUSA()            do { printf("Presione Enter para continuar..."); limpiar_entrada(); } while(0)
#endif

void leer_linea(char *dest, size_t tam);
void limpiar_entrada(void);
int  archivo_existe(const char *path);
int  archivo_copiar(const char *origen, const char *destino);
void salir_y_persistir(t_indice *ind_socios, t_indice *ind_titulos, t_matriz_consumo matriz_consumo,
                       const t_fecha *fecha_proceso);

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

void salir_y_persistir_ALU(t_indice *ind_socios, t_indice *ind_titulos,
                              t_matriz_consumo matriz_consumo, const t_fecha *fecha_proceso);

#endif /* UTIL_H_INCLUDED */
