#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auditoria.h"
#include "lectura.h"
#include "cadena.h"
#include "util.h"
#include "indice.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    agrega al final del archivo de auditoria una linea con el
                formato DNI;TIPO_VALIDACION;FECHA_PROCESO.
Parametros:     path: ruta del archivo de texto de errores.
                dni_texto: DNI tal como vino en la entrada.
                tipo_error: nombre de la validacion que fallo.
                fecha_proceso: fecha de proceso.
Retorno:        OK si se pudo grabar, ERROR en caso contrario.
Observaciones:  El archivo se abre en modo append; si no existe se crea.
******************************************************************************/
int auditoria_registrar_ALU(const char *path, const char *dni_texto,
                        const char *tipo_error, const t_fecha *fecha_proceso)
{
    FILE *arch;

    if (!path || !dni_texto || !tipo_error || !fecha_proceso)
        return ERROR;

    arch = fopen(path, "a");
    if (!arch)
        return ERROR;

    fprintf(arch, "%s;%s;%02d/%02d/%04d\n",
            dni_texto, tipo_error,
            fecha_proceso->dia, fecha_proceso->mes, fecha_proceso->anio);

    fclose(arch);
    return OK;
}

/******************************************************************************
Descripcion:    lee el archivo de auditoria y muestra, por cada par (DNI, tipo
                de validacion), la cantidad de incidencias registradas y el
                total general.
Parametros:     path: ruta del archivo de texto de errores.
Retorno:        OK si se pudo procesar (aunque el archivo no exista), ERROR si
                fallo la memoria.
Observaciones:  Acumula en un vector dinamico que crece con realloc.
******************************************************************************/
#define TAM_DNI_AUDITORIA     32
#define CAP_INICIAL_AUDITORIA 10

int auditoria_mostrar_ALU(const char *path)
{
    typedef struct
    {
        char     dni[TAM_DNI_AUDITORIA];
        char     tipo[TAM_TIPO_ERROR];
        unsigned cantidad;
    } t_incidencia;

    FILE         *arch;
    t_incidencia *vec = NULL, *aux;
    unsigned      cant = 0, cap = 0, total = 0, i;
    char          linea[TAM_LINEA];
    char         *campos[3];
    int           encontrado;

    arch = fopen(path, "rt");
    if(!arch)
    {
        printf("No hay errores registrados (%s no existe).\n", path);
        return OK;
    }

    while(fgets(linea, sizeof(linea), arch))
    {
        quitar_salto(linea);
        if(cadena_vacia(linea))
            continue;

        if(separar_campos_ALU(linea, campos, 3) < 2)
            continue;

        encontrado = 0;
        for(i = 0; i < cant && !encontrado; i++)
        {
            if(strcmp(vec[i].dni, campos[0]) == 0 && strcmp(vec[i].tipo, campos[1]) == 0)
            {
                vec[i].cantidad++;
                encontrado = 1;
            }
        }

        if(!encontrado)
        {
            if(cant == cap)
            {
                cap = cap ? cap * 2 : CAP_INICIAL_AUDITORIA;
                aux = realloc(vec, cap * sizeof(t_incidencia));
                if(!aux)
                {
                    printf("Sin memoria para procesar la auditoria.\n");
                    free(vec);
                    fclose(arch);
                    return ERROR;
                }
                vec = aux;
            }
            copiar_cadena(vec[cant].dni, campos[0], sizeof(vec[cant].dni));
            copiar_cadena(vec[cant].tipo, campos[1], sizeof(vec[cant].tipo));
            vec[cant].cantidad = 1;
            cant++;
        }
        total++;
    }
    fclose(arch);

    if(cant == 0)
        printf("No hay incidencias registradas.\n");
    else
    {
        printf("%-15s %-25s %10s\n", "DNI", "TIPO VALIDACION", "CANTIDAD");
        for(i = 0; i < cant; i++)
            printf("%-15s %-25s %10u\n", vec[i].dni, vec[i].tipo, vec[i].cantidad);
    }
    printf("\nTotal de incidencias: %u\n", total);

    free(vec);
    return OK;
}
