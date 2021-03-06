#include "memoriacompartida.h"

#include <memory.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>


int cmpMemCrear(size_t tamano, int idMemoria)
{
	key_t clave = ftok(RUTAMEMORIACOMPARTIDA, idMemoria);

	if(clave == -1)
		return MEMERRORTOK;

	int id = shmget(clave, tamano, IPC_CREAT | IPC_EXCL | 0666);

	if(id == -1)	
		return MEMERRORCREACION;

	return id;
}

int cmpMemObtener(size_t tamano, int idMemoria)
{
	key_t clave = ftok(RUTAMEMORIACOMPARTIDA, idMemoria);

	if(clave == -1)
		return MEMERRORTOK;

	int id = shmget(clave, tamano, IPC_CREAT | 0666);

	if(id == -1)
		return MEMERRORCREACION;

	return id;
}

int cmpMemDesvincular(void* memoria){
	return shmdt(memoria);
}

int cmpMemDestruir(int idMemoria, void* memoria)
{
	int error = shmdt(memoria);
	shmctl(idMemoria, IPC_RMID, NULL);	

	if( error == -1)
		return MEMERRORDESTRUIR;

	return MEMOK;
}

void *cmpMemObtenerMemoria(int idMemoria)
{
	return shmat(idMemoria, NULL, 0);
}
