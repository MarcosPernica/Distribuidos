#include "memoriacompartida.h"

#include <memory.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>


int cmpMemCrear(struct cmpMem &memoria, size_t tamaño, int idMemoria)
{
	if(memoria.memoria != NULL)
		return MEMERRORYACREADA;

	memoria.id = -1;
	memoria.memoria = NULL;
	memoria.tamaño = 0;

	key_t clave = ftok(RUTAMEMORIACOMPARTIDA, idMemoria);

	if(clave == -1)
		return MEMERRORTOK;

	int id = shmget(clave, tamaño, IPC_CREAT | 0666);

	if(id == -1)	
		return MEMERRORCREACION;

	memoria.id = id;

	void *mem = shmat(memoria.id, NULL, 0);

	if(mem == (void*)-1)
		return MEMERRORMAPEO;

	memoria.memoria = mem;
	memoria.tamaño = tamaño;

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
			memoria.tamaño = 0;
			
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
	
	memcpy(buffer, memoria.memoria, memoria.tamaño);

	return MEMOK;
}

int cmpMemEscribir(struct cmpMem &memoria, void* buffer)
{
	if(memoria.memoria == NULL)
		return MEMERRORLECTURA;
	
	memcpy(memoria.memoria, buffer, memoria.tamaño);

	return MEMOK;
}

