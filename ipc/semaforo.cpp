#include "semaforo.h"

int crearSem(int id,  int initialValue)
{
	key_t clave = ftok("/bin/bash", id);
	if( clave == -1 )
	{
		return clave;
	}
	int semId = semget(clave, 1, IPC_CREAT | IPC_EXCL | 0666 );
	if( semId == -1 )
	{
		return semId;
	}
	union semun sem;
	sem.val = initialValue;
	if(semctl(id, 0, SETVAL, sem) == -1){
		destruirSem(semId);
		return -1;
	}
	return semId;

}

int obtenerSem(int id){
	key_t clave = ftok("/bin/bash", id);
	if(clave == -1){
		return clave;
	}
	return semget(clave, 1, IPC_CREAT | 0666 );
}

int tomarSem(int id)
{
	struct sembuf operacion;

	operacion.sem_num = 0;
	operacion.sem_flg = 0;
	operacion.sem_op = -1;

	return semop(id, &operacion, 1);
}

int liberarSem(int id)
{
	struct sembuf operacion;
	operacion.sem_num = 0;
	operacion.sem_flg = 0;
	operacion.sem_op = 1;
	return semop(id, &operacion, 1);
}

int esperarACero(int id)
{
	struct sembuf operacion;
	operacion.sem_num = 0;
	operacion.sem_flg = 0;
	operacion.sem_op = 0;
	return semop(id, &operacion, 1);
}

int destruirSem(int id)
{
	return semctl(id, 0, IPC_RMID);	
}
