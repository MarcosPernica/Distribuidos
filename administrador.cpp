#include "stdlib.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "mensajes.h"
#include "common.h"
#include "administrador.h"
#include "baseDeDatos/baseDeDatos.h"

sig_atomic_t admin_vivo = 0;

void terminarAdmin(int sigint){
	admin_vivo = 0;
}

int procesarMensaje(const mensaje &recibido, int colaEnvio, struct db &baseDeDatos){
	mensaje respuesta;
	respuesta.mtype = recibido.mtype; 	
	respuesta.tipoMensaje = recibido.tipoMensaje;

	switch(recibido.tipoMensaje)
		{
			case PEDIR_SALAS:
				respuesta.salaP.salas = dbObtenerSalas(baseDeDatos);
				break;
			case ELEGIR_SALA:
				dbEntrarSala(baseDeDatos, recibido.SalaE);				
				break;
			case INTERACCION_ASIENTO:
				break;
			case FINALIZAR_COMPRA:
				break;
			default:
				break;
				//bad msg
		}

	//todo procesar y enviar a todos
	return enviarMensaje(colaEnvio,(void*)&respuesta,sizeof(respuesta));
}

void correrAdministrador(){
	//Crea la base de datos vacia.
	struct db baseDeDatos;
	if(!dbCrear(baseDeDatos))
	{
		printf("Admin error al crear base de datos");
		exit(1);
	}

	//Obtiene la cola de recepcion ADMIN_CINEHIJO.
	int colaRecepcion = obtenerCola(COLA_RECEPCION_ADMIN);
	if (colaRecepcion < 0){
		printf("Admin error al obtener cola");
		exit(1);
	}
	//Obtiene la cola de envio ADMIN_CINEHIJO.
	int colaEnvio = obtenerCola(COLA_ENVIO_ADMIN);
	if (colaEnvio < 0){
		printf("Admin error al obtener cola");
		exit(1);
	}
	
	//Obtiene la cola para enviar al cliente los cambios en las salas.
	int colaEnvio = obtenerCola(COLA_ASINC_CLIENTE);
	if (colaEnvio < 0){
		printf("Admin error al obtener cola");
		exit(1);
	}
	
	//Registra la señal de cerrado del admin.
	int registro = registrarSenal(SIGINT,terminarAdmin);
	if (registro < 0){
		printf("Admin error al obtener cola");
		exit(1);
	}

	mensaje recibido;
	while(admin_vivo){
		int result = recibirMensaje(colaRecepcion,(void*)&recibido,sizeof(recibido));
		if (result == -1){
			printf("Admin error al obtener cola");
			break;
		}
		if ( procesarMensaje(recibido, colaEnvio, baseDeDatos) < 0){
			printf("Admin error al enviar respuestas");
			break;
		}
	}

	exit(0);
}
