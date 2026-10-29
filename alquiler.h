#ifndef ALQUILER_H_INCLUDED
#define ALQUILER_H_INCLUDED

#include "socio.h"
#include "titulo.h"

typedef int t_matriz_consumo[CANT_PLANES][CANT_GENEROS];

void matriz_consumo_inicializar(t_matriz_consumo m);
int  matriz_consumo_persistir(t_matriz_consumo m, const char *path);
void tablero_mostrar(t_matriz_consumo m);
int  alquilar(t_indice *ind_socios, t_indice *ind_titulos, const char *path_socios,
              const char *path_titulos, t_matriz_consumo matriz_consumo, const t_fecha *fecha_proceso);

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

void tablero_mostrar_ALU(t_matriz_consumo m);
int alquilar_ALU(t_indice *ind_socios, t_indice *ind_titulos, const char *path_socios,
             const char *path_titulos, t_matriz_consumo matriz_consumo, const t_fecha *fecha_proceso);

#endif /* ALQUILER_H_INCLUDED */
