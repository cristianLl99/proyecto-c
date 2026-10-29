#ifndef VALIDACIONES_H_INCLUDED
#define VALIDACIONES_H_INCLUDED

/* Reglas de negocio puras: no dependen del indice ni de los archivos */

/* ---- CUIL (Anexo A.1) ---- */
#define TAM_CUIL 14      /* "XY-NNNNNNNN-Z" + '\0' */
#define PREFIJO_HOMBRE 20
#define PREFIJO_MUJER  27
#define PREFIJO_RESTO1 23

void calcular_cuil(long dni, char sexo, char *dest);

/* ---- Email (Anexo A.3) ---- */
#define TAM_EMAIL     31     /* 30 caracteres + '\0' */
#define EMAIL_MAX_LEN 30

int email_valido(const char *);

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

void calcular_cuil_ALU(long dni, char sexo, char *dest);
int email_valido_ALU(const char *email);

#endif /* VALIDACIONES_H_INCLUDED */
