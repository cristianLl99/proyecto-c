/******************************************************************************/
/*  Topicos de Programacion - Trabajo Practico 2C2026                         */
/*  Videoclub "Cinefilia"                                                     */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "util.h"
#include "menu.h"
#include "fecha.h"
#include "indice.h"
#include "lectura.h"
#include "socio.h"
#include "titulo.h"
#include "novedad.h"
#include "alquiler.h"
#include "auditoria.h"
#include "generador.h"

#define MENU_GENERAR   '1'
#define MENU_ABM_SOCIO '2'
#define MENU_BAJA      '3'
#define MENU_NOVEDADES '4'
#define MENU_ALQUILER  '5'
#define MENU_TABLERO   '6'
#define MENU_EXPORTAR  '7'
#define MENU_LST_SOC   '8'
#define MENU_LST_TIT   '9'
#define MENU_AUDITORIA 'A'
#define MENU_GENERAR_PRUEBA 'G'
#define MENU_SALIR     'S'

static const char OPCIONES[][LENGTH_MENU] =
{
    "123456789AGS",
    "Generar archivos binarios desde CSV",
    "Alta / Modificacion manual de socio",
    "Baja logica de socio",
    "Procesar lote de novedades de catalogo",
    "Alquiler de un titulo",
    "Tablero de control gerencial (matriz de consumo)",
    "Exportar socios seleccionados (beta testers)",
    "Listado de socios activos ordenados por DNI",
    "Listado de titulos disponibles",
    "Visualizar auditoria de errores de socios",
    "Generar archivos de prueba (CSV y novedades)",
    "Salir y persistir"
};

/******************************************************************************
Descripcion:    punto de entrada. Pide la fecha de proceso, crea los indices y
                los carga desde los binarios si existen, inicializa la matriz
                de consumo y ejecuta el menu principal hasta la opcion Salir.
Parametros:     No posee.
Retorno:        0 al finalizar normalmente, 1 si no hay memoria para los
                indices.
Observaciones:  Mientras no haya datos cargados solo se habilitan Generar
                binarios, Auditoria, Generar archivos de prueba y Salir.
******************************************************************************/
int main(void)
{
    t_fecha  fecha_proceso;
    t_indice ind_socios, ind_titulos;
    t_matriz_consumo matriz_consumo;
    int      datos_cargados;
    char     op;

    /*************************************************************/
    /*               Ingreso la Fecha de Proceso                 */
    /*************************************************************/
    pedir_fecha_proceso_ALU(&fecha_proceso);

    /*************************************************************/
    /*        Creo los indices y los cargo si hay binarios       */
    /*************************************************************/
    if(indice_crear_ALU(&ind_socios, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice)) != OK ||
       indice_crear_ALU(&ind_titulos, CANTIDAD_ELEMENTOS, sizeof(t_reg_indice)) != OK)
    {
        printf("Sin memoria para los indices.\n");
        return 1;
    }
    datos_cargados = cargar_indices_ALU(&ind_socios, &ind_titulos);
    if(datos_cargados)
        printf("Indices cargados: %u socios activos, %u titulos activos.\n",
               ind_socios.cantidad_elementos_actual, ind_titulos.cantidad_elementos_actual);
    else
        printf("No se encontraron archivos binarios. Ejecute la opcion 1 para generarlos.\n");
    matriz_consumo_inicializar(matriz_consumo);
    PAUSA();

    /*************************************************************/
    /*                     Menu principal                        */
    /*************************************************************/
    do
    {
        LIMPIAR_PANTALLA();
        printf("Fecha de proceso: %02d/%02d/%04d\n\n", fecha_proceso.dia, fecha_proceso.mes, fecha_proceso.anio);
        op = menu(OPCIONES, "Videoclub Cinefilia - Menu Principal");
        printf("\n");

        if(op != MENU_GENERAR && op != MENU_SALIR && op != MENU_AUDITORIA && op != MENU_GENERAR_PRUEBA && !datos_cargados)
        {
            printf("No hay datos cargados. Ejecute primero la opcion 1.\n");
            PAUSA();
            continue;
        }

        switch(op)
        {
            case MENU_GENERAR:
                datos_cargados = generar_binarios_ALU(&ind_socios, &ind_titulos, &fecha_proceso);
                break;
            case MENU_ABM_SOCIO:
                abm_socio_ALU(&ind_socios, &fecha_proceso);
                break;
            case MENU_BAJA:
                socio_baja_ALU(&ind_socios, ARCH_SOCIOS_BIN);
                break;
            case MENU_NOVEDADES:
                novedades_procesar_ALU(&ind_titulos, ARCH_NOVEDADES, ARCH_TITULOS_BIN, &fecha_proceso);
                break;
            case MENU_ALQUILER:
                alquilar_ALU(&ind_socios, &ind_titulos, ARCH_SOCIOS_BIN, ARCH_TITULOS_BIN, matriz_consumo, &fecha_proceso);
                break;
            case MENU_TABLERO:
                tablero_mostrar_ALU(matriz_consumo);
                break;
            case MENU_EXPORTAR:
                socios_exportar_ALU(&ind_socios, ARCH_SOCIOS_BIN, ARCH_BETA_TESTERS, &fecha_proceso);
                break;
            case MENU_LST_SOC:
                socios_listar_ALU(&ind_socios, ARCH_SOCIOS_BIN);
                break;
            case MENU_LST_TIT:
                titulos_listar_ALU(&ind_titulos, ARCH_TITULOS_BIN);
                break;
            case MENU_AUDITORIA:
                auditoria_mostrar_ALU(ARCH_ERRORES);
                break;
            case MENU_GENERAR_PRUEBA:
                generar_archivos_prueba();
                break;
            case MENU_SALIR:
                salir_y_persistir_ALU(&ind_socios, &ind_titulos, matriz_consumo, &fecha_proceso);
                break;
        }
        if(op != MENU_SALIR)
            PAUSA();
    } while(op != MENU_SALIR);

    return 0;
}
