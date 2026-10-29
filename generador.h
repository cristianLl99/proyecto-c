#ifndef GENERADOR_H_INCLUDED
#define GENERADOR_H_INCLUDED

/* Generacion de archivos de prueba. La implementacion se distribuye como   */
/* biblioteca (libgenerador.a); los alumnos solo disponen de este header.   */

int generar_socios_csv(const char *path);
int generar_titulos_csv(const char *path);
int generar_novedades_catalogo(const char *path);
int generar_archivos_prueba(void);     /* los tres, en las rutas de config.h */

#endif /* GENERADOR_H_INCLUDED */
