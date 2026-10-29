#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "fecha.h"
#include "util.h"
#include "indice.h"      /* OK / ERROR */

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    solicita la fecha de proceso por teclado hasta obtener una
                valida. Si el usuario solo presiona Enter toma la fecha actual
                del sistema.
Parametros:     fecha_proceso: fecha destino.
Retorno:        No posee.
Observaciones:  -
******************************************************************************/
void pedir_fecha_proceso_ALU(t_fecha *fecha_proceso)
{
    int resultado;

    do
    {
        printf("Ingrese la fecha de proceso (dd/mm/aaaa) o Enter para hoy: ");
        resultado = lee_fecha(fecha_proceso);

        if (resultado == FECHA_VACIA)
        {
            fecha_actual(fecha_proceso);
        }
        else if (resultado == ERROR)
        {
            printf("Fecha invalida. Intente nuevamente.\n");
        }
    }
    while ( resultado == ERROR);
}
