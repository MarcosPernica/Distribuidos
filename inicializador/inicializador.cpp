#include "../ipc/cola.h"
#include "../common.h"
#include "stdio.h"
#include "stdlib.h"

void tryCreateCola(int id){
	if ( crearCola(id) == -1 ){
		printf("Error al crear cola %d\n",id);
		exit(1);
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
}
