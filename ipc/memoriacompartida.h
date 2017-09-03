#ifndef MEMORIACOMPARTIDA
#define MEMORIACOMPARTIDA

#define RUTAMEMORIACOMPARTIDA "/bin/bash"
#define MEMERRORTOK -1
#define MEMERRORCREACION -2
#define MEMERRRORMAPEO -3
#define MEMERRORDESTRUIR -4
#define MEMERRORYACREADA -5
#define MEMOK 0

struct cmpMem
{
	int id;
	void *memoria;
	size_t tamaño;	
}

/*Crear la memoria e inicializa la estructura. Usa para todos la misma ruta de archivo y el idMemoria especificado.*/
int cmpMemCrear(struct cmpMem &memoria, size_t tamaño, int idMemoria);

/*Destruye y desvincula la memoria compartida especificada.
Da error tanto si no se puede desvincular como si la memoria no estaba creada.*/
int cmpMemDestruir(struct cmpMem &memoria);

/*Pone el contenido de la memoria en 'buffer'.*/
int cmpMemLeer(struct cmpMem &memoria, void* buffer);

/*Escribe el contenido de 'buffer' en la memoria.*/
int cmpMemEscribir(struct cmpMem &memoria, void* buffer);

#endif
