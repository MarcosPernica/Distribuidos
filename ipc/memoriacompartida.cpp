#include "memoriacompartida.h"

#include <memory.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>


int cmpMemCrear(struct cmpMem &memoria, size_t tamano, int idMemoria)
{
	if(memoria.memoria != NULL)
		return MEMERRORYACREADA;

	memoria.id = -1;
	memoria.memoria = NULL;
	memoria.tamano = 0;

	key_t clave = ftok(RUTAMEMORIACOMPARTIDA, idMemoria);

	if(clave == -1)
		return MEMERRORTOK;

	int id = shmget(clave, tamano, IPC_CREAT | IPC_EXCL | 0666);

	if(id == -1)	
		return MEMERRORCREACION;

	memoria.id = id;

	void *mem = shmat(memoria.id, NULL, 0);

	if(mem == (void*)-1)
		return MEMERRORMAPEO;

	memoria.memoria = mem;
	memoria.tamano = tamano;

	return MEMOK;
}

int cmpMemObtener(struct cmpMem &memoria, size_t tamano, int idMemoria)
{
	if(memoria.memoria != NULL)
		return MEMERRORYACREADA;

	memoria.id = -1;
	memoria.memoria = NULL;
	memoria.tamano = 0;

	key_t clave = ftok(RUTAMEMORIACOMPARTIDA, idMemoria);

	if(clave == -1)
		return MEMERRORTOK;

	int id = shmget(clave, 0, IPC_CREAT | 0666);

	if(id == -1)
		return MEMERRORCREACION;

	memoria.id = id;

	void *mem = shmat(memoria.id, NULL, 0);

	if(mem == (void*)-1)
		return MEMERRORMAPEO;

	memoria.memoria = mem;
	memoria.tamano = tamano;

	return MEMOK;
}

int cmpMemDestruir(struct cmpMem &memoria)
{
	if(memoria.id != -1)
	{
		shmctl(memoria.id, IPC_RMID, NULL);
		memoria.id = -1;
	}

	if(memoria.memoria)
	{
		if(shmdt(memoria.memoria) == -1)
			return MEMERRORDESTRUIR;
		else
		{
			memoria.memoria = NULL;
			memoria.tamano = 0;
			
		}
	}

	return MEMERRORDESTRUIR;
}

void *cmpMemObtenerMemoria(struct cmpMem &memoria)
{
	return memoria.memoria;
}

int cmpMemLeer(struct cmpMem &memoria, void* buffer)
{
	if(memoria.memoria == NULL)
		return MEMERRORLECTURA;
	
	memcpy(buffer, memoria.memoria, memoria.tamano);

	return MEMOK;
}

int cmpMemEscribir(struct cmpMem &memoria, void* buffer)
{
	if(memoria.memoria == NULL)
		return MEMERRORESCRITURA;
	
	memcpy(memoria.memoria, buffer, memoria.tamano);

	return MEMOK;
}

