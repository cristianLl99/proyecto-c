#include <string.h>
#include "novedad.h"
#include "cadena.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    recorre secuencialmente novedades_catalogo.dat y aplica cada
                operacion sobre el indice y titulos.dat: 'A' agrega si el ID
                no esta en el indice (validando como en la carga), 'B' pone
                estado 'B' en el archivo y elimina del indice, 'M' reemplaza
                titulo y genero y suma el stock (rechaza si quedaria
                negativo). Al final muestra el reporte de procesadas y
                rechazadas.
Parametros:     indice: indice de titulos.
                path_novedades: ruta de novedades_catalogo.dat.
                path_titulos: ruta de titulos.dat.
                fecha_proceso: fecha de proceso (para el contexto de
                validacion).
Retorno:        OK si se pudieron abrir los archivos, ERROR en caso contrario.
Observaciones:  Como el indice solo tiene activos, un 'A' sobre un ID dado de
                baja se agrega como registro nuevo al final del archivo.
******************************************************************************/
int novedades_procesar_ALU(t_indice *indice, const char *path_novedades,
                       const char *path_titulos, const t_fecha *fecha_proceso)
{
    FILE *fnov, *ftit;
    t_novedad nov;
    t_titulo tit;
    t_contexto ctx;
    t_reg_indice reg_ind;
    char tipo_error[TAM_TIPO_ERROR];
    const char *motivo;
    long fin_pos;
    unsigned procesadas = 0, rechazadas = 0;

    fnov = fopen(path_novedades, "rb");
    if (!fnov)
        return ERROR;

    ftit = fopen(path_titulos, "r+b");
    if (!ftit)
    {
        ftit = fopen(path_titulos, "w+b");
        if (!ftit)
        {
            fclose(fnov);
            return ERROR;
        }
    }

    ctx.indice = indice;
    ctx.arch_bin = ftit;
    ctx.fecha_proceso = fecha_proceso;
    ctx.id_modificando = 0;

    printf("\nProcesando %s...\n\n", path_novedades);

    while (fread(&nov, sizeof(t_novedad), 1, fnov) == 1)
    {
        motivo = NULL;
        reg_ind.id = clave_titulo(&nov.titulo);
        reg_ind.nro_reg = 0;
        int pos = indice_buscar_ALU(indice, &reg_ind, sizeof(t_reg_indice),
                                    compara_reg_indice_ALU);

        switch (nov.codigo_operacion)
        {
        case OP_ALTA://¿este ID ya existe y está activo?
            if (pos != NO_EXISTE)
            {
                motivo = "el ID ya existe";
                break;
            }
            nov.titulo.estado = 'A';
            tipo_error[0] = '\0';
            if (valida_titulo_ALU(&nov.titulo, &ctx, tipo_error) != OK)
            {
                motivo = tipo_error;
                break;
            }
            fseek(ftit, 0, SEEK_END);
            fin_pos = ftell(ftit);
            if (fwrite(&nov.titulo, sizeof(t_titulo), 1, ftit) != 1)
            {
                motivo = "error de escritura";
                break;
            }
            fflush(ftit);
            reg_ind.nro_reg = (unsigned)(fin_pos / (long)sizeof(t_titulo));
            if (indice_insertar_ALU(indice, &reg_ind, sizeof(t_reg_indice),
                                    compara_reg_indice_ALU) != OK)
            {
                motivo = "error al insertar en el indice";
                break;
            }
            printf("  AGREGADA   id=%-6d %s\n", nov.titulo.id, nov.titulo.titulo);
            break;

        case OP_BAJA:
            if (pos == NO_EXISTE)
            {
                motivo = "el ID no existe o ya esta dado de baja";
                break;
            }
            fseek(ftit, (long)reg_ind.nro_reg * (long)sizeof(t_titulo), SEEK_SET);
            if (fread(&tit, sizeof(t_titulo), 1, ftit) != 1)
            {
                motivo = "error de lectura";
                break;
            }
            tit.estado = 'B';
            fseek(ftit, (long)reg_ind.nro_reg * (long)sizeof(t_titulo), SEEK_SET);
            if (fwrite(&tit, sizeof(t_titulo), 1, ftit) != 1)
            {
                motivo = "error de escritura";
                break;
            }
            fflush(ftit);
            indice_eliminar_ALU(indice, &reg_ind, sizeof(t_reg_indice),
                                compara_reg_indice_ALU);
            printf("  BAJA       id=%-6d %s\n", tit.id, tit.titulo);
            break;

        case OP_MODIFICACION:
            if (pos == NO_EXISTE)
            {
                motivo = "el ID no existe o esta dado de baja";
                break;
            }
            fseek(ftit, (long)reg_ind.nro_reg * (long)sizeof(t_titulo), SEEK_SET);
            if (fread(&tit, sizeof(t_titulo), 1, ftit) != 1)
            {
                motivo = "error de lectura";
                break;
            }
            if (tit.stock + nov.titulo.stock < 0)
            {
                motivo = "STOCK (quedaria negativo)";
                break;
            }
            arreglar_titulo(nov.titulo.titulo);   /* igual que en la carga */
            strcpy(tit.titulo, nov.titulo.titulo);
            strcpy(tit.genero, nov.titulo.genero);
            tit.stock += nov.titulo.stock;
            fseek(ftit, (long)reg_ind.nro_reg * (long)sizeof(t_titulo), SEEK_SET);
            if (fwrite(&tit, sizeof(t_titulo), 1, ftit) != 1)
            {
                motivo = "error de escritura";
                break;
            }
            fflush(ftit);
            printf("  MODIFICADA id=%-6d %s (stock %d)\n", tit.id, tit.titulo, tit.stock);
            break;

        default:
            motivo = "codigo de operacion invalido";
            break;
        }

        if (motivo)
        {
            rechazadas++;
            printf("  RECHAZADA  op=%c id=%-6d motivo: %s\n",
                   nov.codigo_operacion, nov.titulo.id, motivo);
        }
        else
            procesadas++;
    }

    printf("\nNovedades procesadas: %u   Rechazadas: %u\n", procesadas, rechazadas);

    fclose(fnov);
    fclose(ftit);
    return OK;
}
