#include "stdlib.h"
#include "cinehijo.h"
#include "ipc/cola.h"
#include "common.h"
#include "ipc/senal.h"

sig_atomic_t hijo_vivo = 0;

void terminarHijo(int sigint){
	hijo_vivo = 1;
}

void procesarMensaje(mensaje recibido, mensaje * respuesta){
	switch(recibido.tipoMensaje)
	{
		case PEDIR_SALAS:
			break;
		case ELEGIR_SALA:
			break;
		case ELEGIR_ASIENTO:
			break;
		case FINALIZAR_COMPRA:
			break;
		default:
			break;
			//bad msg
	}
}

void administrarCliente(login login){
	if (registrarSenal(SIGINT,terminarHijo) == -1){
		printf("Hijo: error al registrar senal");
		exit(1);
	}
	int pid = login.id;
	mensaje msg;
	int colaId = obtenerCola(COLA_RECEPCION_CINE);
	if (colaId == -1){
		printf("Hijo error al obtener cola recepcion");
		exit(1);
	}

	int colaEnvio = obtenerCola(COLA_ENVIO_CINE);
	if (colaEnvio == -1){
			printf("Hijo error al obtener cola envio");
			exit(1);
		}

	while (hijo_vivo == 0)
	{
		if (recibirMensaje(colaId,pid,(void*)&msg,sizeof(msg)) == -1)
		{
			//handle error
			printf("Hijo error al recibir mensaje");
			break;
		}
		mensaje respuesta;
		respuesta.mtype = pid;
		procesarMensaje(msg,&respuesta);
		if (enviarMensaje(colaEnvio,(void*)&respuesta,sizeof(respuesta)) == -1)
		{
			//handle error
			printf("Hijo error al enviar mensaje");
			break;
		}
	}
}
