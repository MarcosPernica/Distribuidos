#include "../ipc/cola.h"
#include "../ipc/semaforo.h"
#include "../ipc/memoriacompartida.h"
#include "../cineAsyncHandler/cineAsyncHandler.h"
#include "../common.h"
#include "stdio.h"
#include "stdlib.h"

void tryCreateCola(int id){
	int colaId = -1;
	if ( (colaId = crearCola(id)) == -1 ){
		printf("Error al crear cola %d\n",id);
		exit(1);
	} else
	{
		printf("Cola %i tiene id %i\n",id,colaId);
	}
}

int main(int argc, char** argv)
{
	tryCreateCola(COLA_LOGIN_CINE);
	tryCreateCola(COLA_RECEPCION_CINE);
	tryCreateCola(COLA_ENVIO_CINE);
	tryCreateCola(COLA_ASINC_CLIENTE);
	tryCreateCola(COLA_RECEPCION_ADMIN);
	tryCreateCola(COLA_ENVIO_ADMIN);
	tryCreateCola(COLA_MOM_CLIENT);
	tryCreateCola(COLA_CLIENT_MOM);

	if( crearSem(MUTEX_CINE_ID,1) < -1 ){
		printf("Semaforo cine ya creado\n");
		exit(1);
	}

	if( cmpMemCrear(sizeof(struct socketMapper), MEMORIA_COMPARTIDA_CINE_ID) < 0 ){
		printf("No pudo crear memoria compartida del cine\n");
		exit(1);
	}
}
