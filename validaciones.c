#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "validaciones.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    calcula el CUIL con el algoritmo modulo 11 del Anexo A.1:
                prefijo 20 (hombre) o 27 (mujer), DNI a 8 digitos y digito
                verificador. Si el resto es 1 el prefijo pasa a 23 y Z es 9
                (hombre) o 4 (mujer).
Parametros:     dni: numero de documento.
                sexo: 'H' o 'M'.
                dest: buffer de al menos TAM_CUIL bytes donde se deja "XY-
                NNNNNNNN-Z".
Retorno:        No posee.
Observaciones:  -
******************************************************************************/
void calcular_cuil_ALU(long dni, char sexo, char *dest)
{
    int digitos[10];
    int prefijo,dni_suma=0,resto,z,i;
    long dniAux=dni;
    int mult[10]= {5, 4, 3, 2, 7, 6, 5, 4, 3, 2};


    if(sexo =='M')

        prefijo=PREFIJO_MUJER;

    else if(sexo=='H')

        prefijo=PREFIJO_HOMBRE;

    else
    {
        dest[0] = '\0';
        return;
    }

    for ( i = 9; i >= 2; i--)
    {
        digitos[i] = dniAux % 10;
        dniAux /= 10;
    }
    digitos[0]=prefijo/10;
    digitos[1]=prefijo%10;

    for(i=0; i<10; i++)
    {
        dni_suma += digitos[i] * mult[i];
    }
    resto = dni_suma % 11;

    if(resto == 0)
    {
        z=0;
    }
    else if(resto==1)
    {
        prefijo = PREFIJO_RESTO1;
        z = (sexo == 'H') ? 9 : 4;
    }
    else
    {
        z=11-resto;
    }

    sprintf(dest,"%02d-%08ld-%d",prefijo,dni,z);
}

/******************************************************************************
Descripcion:    valida un email segun el Anexo A.3: hasta 30 caracteres sin
                espacios, una unica '@', parte local de letras/digitos/puntos
                que no empieza ni termina con punto, y dominio de
                letras/digitos/guiones/puntos con al menos un punto, que no
                empieza ni termina con punto o guion y cuyo ultimo segmento
                tiene al menos 2 letras.
Parametros:     email: cadena a validar.
Retorno:        1 si es valido, 0 en caso contrario.
Observaciones:  No verifica unicidad; eso lo hace socio_email_existe.
******************************************************************************/
#define ES_DIGITO(c)  ( (c) >= '0' && (c) <= '9' )
#define ES_LETRA(c)   ( ((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') )
int email_valido_ALU(const char *email)
{
    size_t len=strlen(email);
    const char *pemail;
    const char *ult_punto;
    const char *arroba= strchr(email, '@');
    const char *dom;
    const char *fin = email+len;
    int cant_letras;


    if(len > EMAIL_MAX_LEN)
        return 0;

    pemail=email;
    while(*pemail != '\0')
    {
        if( *pemail == ' ')
        {
            return 0;

        }
        pemail++;

    }

    //busco @ en el mail
    if (arroba == NULL)
    {
        return 0;
    }
    dom = arroba + 1;
    if(strchr(arroba +1,'@')!= NULL  )
    {
        return 0;
    }
    if(arroba == email)
    {
        return 0;
    }
//parte local


    if(*email == '.' || *(arroba -1) == '.')
    {
        return 0;
    }

    pemail=email;
    while(pemail<arroba)
    {
        if (!ES_LETRA(*pemail) && !ES_DIGITO(*pemail) && *pemail != '.')
            return 0;
        pemail ++;
    }

    //parte dominio


    if(dom>=fin)
    {
        return 0;
    }
    if(*dom == '.' || *dom == '-')
    {
        return 0;
    }
    if(*(fin -1) == '.' || *(fin -1)=='-')
    {
        return 0;
    }
    pemail = dom;
    while (pemail < fin)
    {
        if (!ES_LETRA(*pemail) && !ES_DIGITO(*pemail) && *pemail != '.' && *pemail != '-')
            return 0;
        pemail++;
    }

    //El dominio debe contener al menos un punto
    ult_punto = strrchr(dom, '.');
    if (ult_punto == NULL)
        return 0;

    cant_letras = 0;
    pemail = ult_punto + 1;
    while (pemail < fin)
    {
        if (!ES_LETRA(*pemail))
            return 0;
        cant_letras++;
        pemail++;
    }
    if (cant_letras < 2)
        return 0;

    return 1;
}
