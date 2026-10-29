#ifndef FECHA_H_INCLUDED
#define FECHA_H_INCLUDED

#define TAM_FECHA_TXT 11     /* "dd/mm/aaaa" + '\0' */
#define FECHA_VACIA   2

typedef struct
{
    int dia;
    int mes;
    int anio;
} t_fecha;

int  lee_fecha(t_fecha *);                       /* OK, ERROR o FECHA_VACIA */
int  texto_a_fecha(const char *, t_fecha *);     /* "dd/mm/aaaa" -> t_fecha */
void fecha_a_texto_compacto(const t_fecha *, char *); /* "aaaammdd" */
void fecha_actual(t_fecha *);
int  fecha_correcta(const t_fecha *);
int  cant_dias_mes(int, int);
int  es_bisiesto(int);
int  compara_fechas(const t_fecha *, const t_fecha *); /* <0, 0, >0 */
int  diferencia_dias(const t_fecha *, const t_fecha *);
int  dias_julianos(const t_fecha *);
void suma_anios(t_fecha *, int);
int  edad(const t_fecha *nacimiento, const t_fecha *referencia);

void pedir_fecha_proceso(t_fecha *fecha_proceso);

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

void pedir_fecha_proceso_ALU(t_fecha *fecha_proceso);

#endif /* FECHA_H_INCLUDED */
