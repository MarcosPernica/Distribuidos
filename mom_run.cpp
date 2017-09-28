/*
 * mom_run.cpp
 *
 *  Created on: Sep 26, 2017
 *      Author: tobias
 */
#include "common.h"
#include "apiMensajes/mom.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "mensajes.h"
sig_atomic_t mom_vivo = 0;

void terminar(int sigint){
	mom_vivo = 1;
}

mensaje procesarMensaje(MOM &mom, mensaje lectura)
{
	int fd = lectura.fd;
	mensaje respuesta;
	respuesta.mtype = lectura.mtype;
	respuesta.resultado = 0;
	switch( lectura.tipoMensaje ){
		case INITMOM:
			respuesta.initmom.fd = MOMInitClient(mom,lectura.initmom.pid);
			break;
		case DESTROY_MOM: case SALIR_SALA:
			respuesta.resultado = MOMDeinit(mom,fd);
			break;
		case LOGIN:
			respuesta.resultado = MOMLogin(mom,fd,lectura.l);
			break;
		case PEDIR_SALAS:
			salas salas;
			respuesta.resultado = MOMPedirSalas(mom,fd,salas);
			respuesta.salaP.salas = salas;
			break;
		case ELEGIR_SALA:
			sala sala;
			respuesta.resultado = MOMPedirAsientosSala(mom,fd,lectura.salaE.salaid,sala);
			respuesta.informacionSala = sala;
			break;
		case INTERACCION_ASIENTO:
			respuesta.resultado = MOMElegirAsiento(mom,fd,lectura.asientoS.asiento);
			break;
		case FINALIZAR_COMPRA:
			respuesta.resultado = MOMFinalizarCompra(mom,fd);
			break;
	}
	return respuesta;
}

void MOMDestroy(MOM &mom){
	std::map<int,MOMClient>::iterator it = mom.clients.begin();
	while(it != mom.clients.end()){
		MOMDeinit(mom,it->first);
	}
	mom.clients.clear();
}

int main(int argc, char** argv){
	MOM mom;

	if( registrarSenal(SIGINT,terminar) == -1){
		printf("MOM: error al registrar senal");
		exit(1);
	}
	if( ! MOMInit(mom) ){
		printf("No se puede inicializar el mom\n");
		return 1;
	}
	int colaLectura = obtenerCola(COLA_CLIENT_MOM);
	int colaEscritura = obtenerCola(COLA_MOM_CLIENT);
	mensaje lectura;
	mensaje respuesta;
	while( mom_vivo == 0 ){
		if( recibirMensaje(colaLectura,(void*)&lectura,sizeof(mensaje)) == -1){
			break;
		}
		respuesta = procesarMensaje(mom,lectura);
		if( enviarMensaje(colaEscritura,(void*)&respuesta,sizeof(mensaje)) == -1){
			break;
		}
	}
	//should kill all asinc..
	MOMDestroy(mom);
	return 0;
}
