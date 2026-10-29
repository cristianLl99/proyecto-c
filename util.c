#include <string.h>
#include "util.h"
#include "cadena.h"
#include "config.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    opcion Salir: copia socios.dat y titulos.dat con la fecha de
                proceso en el nombre, vuelca la matriz de consumo a disco y
                libera la memoria de ambos indices.
Parametros:     ind_socios: indice de socios.
                ind_titulos: indice de titulos.
                matriz: matriz de consumo.
                fecha_proceso: fecha de proceso.
Retorno:        No posee.
Observaciones:  -
******************************************************************************/
void salir_y_persistir_ALU(t_indice *ind_socios, t_indice *ind_titulos,
                           t_matriz_consumo matriz_consumo, const t_fecha *fecha_proceso)
{
    char fecha_txt[TAM_FECHA_TXT];
    char nombre[TAM_LINEA];

    fecha_a_texto_compacto(fecha_proceso, fecha_txt);

    sprintf(nombre, "%s%s.dat", PREFIJO_SOCIOS_BKP, fecha_txt);
    printf("%s -> %s\n", ARCH_SOCIOS_BIN,
           archivo_copiar(ARCH_SOCIOS_BIN, nombre) ? nombre : "(no se pudo copiar)");

    sprintf(nombre, "%s%s.dat", PREFIJO_TITULOS_BKP, fecha_txt);
    printf("%s -> %s\n", ARCH_TITULOS_BIN,
           archivo_copiar(ARCH_TITULOS_BIN, nombre) ? nombre : "(no se pudo copiar)");

    sprintf(nombre, "%s%s.dat", PREFIJO_MATRIZ, fecha_txt);
    printf("matriz de consumo -> %s\n",
           matriz_consumo_persistir(matriz_consumo, nombre) ? nombre : "(no se pudo grabar)");

    indice_destruir_ALU(ind_socios);
    indice_destruir_ALU(ind_titulos);
    printf("Memoria liberada. Hasta luego.\n");
}
