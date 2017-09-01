#include "cinehijo.h"
#include "ipc/cola.h"
#include "common.h"

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
	}
}

void administrarCliente(login login){
	int pid = login.id;
	int conectado = 1;
	mensaje msg;
	int colaId = obtenerCola(COLA_RECEPCION_CINE);
	while (conectado)
	{
		if (recibirMensaje(colaId,pid,(void*)&msg,sizeof(msg)) == -1)
		{
			//handle error
			break;
		}
		mensaje respuesta;
		respuesta.mtype = pid;
		procesarMensaje(msg,&respuesta);
		if (enviarMensaje(colaId,(void*)&respuesta,sizeof(respuesta)) == -1)
		{
			//handle error
			break;
		}
	}
}
