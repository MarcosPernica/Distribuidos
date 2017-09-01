#include "ipc/cola.h"
#include "mensajes.h"
#include "common.h"
#include "administrador.h"

bool estaConectado(){
	//todo interrupted
	return true;
}

void procesarMensaje(mensaje recibido, int colaEnvio){
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
	enviarMensaje(colaEnvio,(void*)&respuesta,sizeof(respuesta));
}

void correrAdministrador(){
	int colaRecepcion = obtenerCola(COLA_RECEPCION_ADMIN);
	int colaEnvio = obtenerCola(COLA_ASINC_CLIENTE);
	mensaje recibido;
	while(estaConectado()){
		int result = recibirMensaje(colaRecepcion,(void*)&recibido,sizeof(recibido));
		if (result == -1){
			//TODO handle error
		}
		procesarMensaje(recibido, colaEnvio);
	}
}
