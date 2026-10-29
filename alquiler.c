#include <stdlib.h>
#include <string.h>
#include "alquiler.h"
#include "cadena.h"
#include "util.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    imprime por pantalla la matriz de consumo con totales por fila
                (plan) y por columna (genero).
Parametros:     m: matriz de consumo.
Retorno:        No posee.
Observaciones:  -
******************************************************************************/
void tablero_mostrar_ALU(t_matriz_consumo m)
{
    int total_col[CANT_GENEROS] = {0}, i,j,k;
    int total=0,total_fila;

    printf("%-10s", "PLAN");
    for(k=0; k<CANT_GENEROS; k++)
    {
        printf("%16s", GENEROS[k]);
    }
    printf("%10s\n", "TOTAL");


    for(i=0; i<CANT_PLANES; i++)
    {
        total_fila=0;
        printf("%-10s", PLANES[i]);
        for(j=0; j<CANT_GENEROS; j++)
        {
            printf("%16d", m[i][j]);
            total_fila += m[i][j];
            total_col[j] += m[i][j];

        }
        printf("%10d\n", total_fila);
        total += total_fila;
    }

    printf("%-10s", "TOTAL");
    for(j=0; j<CANT_GENEROS; j++)
    {
        printf("%16d", total_col[j]);

    }
    printf("%10d\n", total);

}

/******************************************************************************
Descripcion:    ejecuta el flujo de alquiler de un titulo: 1) habilitacion del
                socio (debe estar en el indice), 2) inventario (el titulo debe
                estar en el indice y con stock > 0), 3) descuenta una unidad
                de stock en el archivo, 4) incrementa la celda [plan][genero]
                de la matriz, 5) emite el comprobante.
Parametros:     ind_socios: indice de socios.
                ind_titulos: indice de titulos.
                path_socios: ruta de socios.dat.
                path_titulos: ruta de titulos.dat.
                matriz: matriz de consumo a alimentar.
                fecha_proceso: fecha de proceso para el comprobante.
Retorno:        OK si el alquiler se concreto, ERROR si alguna etapa fallo.
Observaciones:  Si una etapa falla se informa el motivo y no se ejecutan las
                siguientes. No se exige cuota al dia.
******************************************************************************/
int alquilar_ALU(t_indice *ind_socios, t_indice *ind_titulos, const char *path_socios,
                 const char *path_titulos, t_matriz_consumo matriz_consumo, const t_fecha *fecha_proceso)
{
    char buffer [64];
    long dni;
    t_reg_indice busqueda_dni;
    t_reg_indice busqueda_titulo;
    t_socio socio;
    t_titulo titulo;
    int pos_socio,id,pos_titulo,fila,col;

    //dni
    printf("Ingrese DNI del socio: ");
    leer_linea(buffer,sizeof(buffer));
    dni=atol(buffer);

    busqueda_dni.id=dni;
    busqueda_dni.nro_reg=0;

    pos_socio=indice_buscar_ALU(ind_socios,&busqueda_dni,sizeof(t_reg_indice),compara_reg_indice_ALU);

    if(pos_socio == NO_EXISTE)
    {
        printf("Socio inexistente o dado de baja. Alquiler rechazado. \n");
        return ERROR;
    }

    FILE *fsocios =fopen(path_socios,"rb");

    if(!fsocios)
    {
        printf("No se pudo leer el archivo\n");
        return ERROR;
    }

    if(socio_leer_ALU(fsocios,busqueda_dni.nro_reg,&socio) != OK)
    {
        printf("No se pudo leer el socio. Alquiler rechazado.\n");
        fclose(fsocios);
        return ERROR;
    }

    fclose(fsocios);

    //titulo

    printf("Ingrese ID de la pelicula: ");
    leer_linea(buffer,sizeof(buffer));
    id=atoi(buffer);

    busqueda_titulo.id=id;
    busqueda_titulo.nro_reg=0;

    pos_titulo=indice_buscar_ALU(ind_titulos,&busqueda_titulo,sizeof(t_reg_indice),compara_reg_indice_ALU);

    if(pos_titulo == NO_EXISTE)
    {
        printf("Titulo inexistente o dado de baja. Alquiler rechazado.\n");
        return ERROR;
    }

    //stock
    FILE *ftitulo=fopen(path_titulos,"r+b");
    if(!ftitulo)
    {
        printf("No se pudo abrir el archivo\n");
        return ERROR;
    }

    fseek(ftitulo,busqueda_titulo.nro_reg*sizeof(t_titulo),SEEK_SET);

    if(fread(&titulo,sizeof(t_titulo),1,ftitulo) != 1)
    {
        printf("No se pudo leer el titulo. Alquiler rechazado.\n");
        fclose(ftitulo);
        return ERROR;
    }
    if(titulo.stock <= 0)
    {
        printf("Titulo sin stock. Alquiler rechazado.\n");
        fclose(ftitulo);
        return ERROR;
    }

    titulo.stock--;
    fseek(ftitulo,busqueda_titulo.nro_reg*sizeof(t_titulo),SEEK_SET);
    if(fwrite(&titulo,sizeof(t_titulo),1,ftitulo) != 1)
    {
        printf("No se pudo actualizar el stock. Alquiler rechazado.\n");
        fclose(ftitulo);
        return ERROR;
    }
    fflush(ftitulo);


    fila=plan_indice(socio.plan);
    col=genero_indice(titulo.genero);
    if(fila>=0 && col>= 0)
    {
        matriz_consumo[fila][col]++;
    }

    printf("\n========== COMPROBANTE DE ALQUILER ==========\n");
    printf(" Fecha    : %02d/%02d/%04d\n", fecha_proceso->dia, fecha_proceso->mes, fecha_proceso->anio);
    printf(" Socio    : %ld - %s (plan %s)\n", socio.dni, socio.apellidos_nombres, socio.plan);
    printf(" Titulo   : %d - %s (%s)\n", titulo.id, titulo.titulo, titulo.genero);
    printf(" Stock restante: %d\n", titulo.stock);
    printf("=============================================\n");

    fclose(ftitulo);

    return OK;
}
