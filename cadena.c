#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include "cadena.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    normaliza un campo Apellidos y Nombres segun el Anexo A.2 (con
                regla de coma).
Parametros:     s: cadena a normalizar (se modifica in place).
Retorno:        No posee.
Observaciones:  -
******************************************************************************/
void arreglar_apyn_ALU(char *s)
{
    char *aux = malloc(strlen(s) + 3);
    char *lect = s;
    char *esc = aux;
    char *p = s;
    int hay_coma = 0;
    int palabra = 0;
    int vio_coma, primera, coma_puesta = 0;

    if(!aux)
        return;

    while(*p && !hay_coma)
    {
        if(*p == ',')
            hay_coma = 1;
        p++;
    }

    while(*lect)
    {
        vio_coma = 0;
        while(*lect && (isspace((unsigned char)*lect) || *lect == ','))
        {
            if(*lect == ',')
                vio_coma = 1;
            lect++;
        }

        if(*lect)
        {
            if(palabra > 0)
            {
                if(!coma_puesta && (vio_coma || (!hay_coma && palabra == 1)))
                {
                    *esc++ = ',';
                    coma_puesta = 1;
                }
                *esc++ = ' ';
            }
            palabra++;

            primera = 1;
            while(*lect && !isspace((unsigned char)*lect) && *lect != ',')
            {
                *esc++ = (char)(primera ? toupper((unsigned char)*lect)
                                        : tolower((unsigned char)*lect));
                primera = (*lect == '\'');   /* D'Angelo: despues del apostrofe va mayuscula */
                lect++;
            }
        }
    }
    *esc = '\0';
    strcpy(s, aux);
    free(aux);
}

/******************************************************************************
Descripcion:    separa una linea CSV in place por SEPARADOR_CSV, reemplazando
                cada separador por '\0' y guardando en campos[] el puntero al
                inicio de cada campo.
Parametros:     linea: linea a separar (se modifica).
                campos: vector donde se guardan los punteros a cada campo.
                max: cantidad maxima de campos a separar.
Retorno:        Cantidad de campos encontrados (los vacios cuentan). Nunca mas
                de max.
Observaciones:  Si la linea tiene mas separadores que max, el ultimo campo
                conserva el resto de la linea.
******************************************************************************/
int separar_campos_ALU(char *linea, char *campos[], int max)
{
    int cantidad = 0;
    char *paux = linea;
    if (max <= 0 || linea == NULL)
    {
        return 0;
    }
    campos[cantidad] = linea;
    cantidad++;
    while (*paux != '\0' && cantidad < max)
    {
        if (*paux == SEPARADOR_CSV)
        {
            *paux = '\0';
            campos[cantidad] = paux + 1;
            cantidad++;
        }
        paux++;
    }
    return cantidad;
}
