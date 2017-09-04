#include "stdlib.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "mensajes.h"
#include "common.h"
#include "administrador.h"

sig_atomic_t admin_vivo = 0;

void terminarAdmin(int sigint){
	admin_vivo = 1;
}

int procesarMensaje(mensaje recibido, int colaEnvio){
	mensaje respuesta;
	switch(recibido.tipoMensaje)
		{
			case INTERACCION_SALA:
				break;
			case INTERACCION_ASIENTO:
				break;
			default:
				break;
				//bad msg
		}

	//todo procesar y enviar a todos
	return enviarMensaje(colaEnvio,(void*)&respuesta,sizeof(respuesta));
}

void correrAdministrador(){
	int colaRecepcion = obtenerCola(COLA_RECEPCION_ADMIN);
	if (colaRecepcion < 0){
		printf("Admin error al obtener cola");
		exit(1);
	}
	int colaEnvio = obtenerCola(COLA_ASINC_CLIENTE);
	if (colaEnvio < 0){
		printf("Admin error al obtener cola");
		exit(1);
	}
	mensaje recibido;
	int registro = registrarSenal(SIGINT,terminarAdmin);
	if (registro < 0){
		printf("Admin error al obtener cola");
		exit(1);
	}

	while(admin_vivo){
		int result = recibirMensaje(colaRecepcion,(void*)&recibido,sizeof(recibido));
		if (result == -1){
			//TODO handle error
			printf("Admin error al obtener cola");
			break;
		}
		if ( procesarMensaje(recibido, colaEnvio) < 0){
			printf("Admin error al enviar respuestas");
			break;
		}
	}

	exit(0);
}
