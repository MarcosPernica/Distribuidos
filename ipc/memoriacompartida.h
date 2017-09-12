#ifndef MEMORIACOMPARTIDA
#define MEMORIACOMPARTIDA

#include <stdio.h>

#define RUTAMEMORIACOMPARTIDA "/bin/bash"
#define MEMERRORTOK -1
#define MEMERRORCREACION -2
#define MEMERRORMAPEO -3
#define MEMERRORDESTRUIR -4
#define MEMERRORYACREADA -5
#define MEMERRORLECTURA -6
#define MEMERRORESCRITURA -7

#define MEMOK 0

/*Crear la memoria. Usa para todos la misma ruta de archivo y el idMemoria especificado.*/
int cmpMemCrear(size_t tamano, int idMemoria);

/*Obtiene la memoria. Usa para todos la misma ruta de archivo y el idMemoria especificado.*/
int cmpMemObtener(size_t tamano, int idMemoria);

/*Destruye y desvincula la memoria compartida especificada.
Da error tanto si no se puede desvincular como si la memoria no estaba creada.*/
int cmpMemDestruir(int idMemoria, void *memoria);

void * cmpMemObtenerMemoria(int idMemoria);


#endif
