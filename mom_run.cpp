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

bool procesarMensaje(MOM &mom, mensaje lectura, mensaje& respuesta)
{
	int fd = lectura.fd;
	respuesta.mtype = lectura.mtype;
	bool resultado = true;
	switch( lectura.tipoMensaje ){
		case INITMOM:
			respuesta.initmom.fd = MOMInitClient(mom,lectura.initmom.pid);
			break;
		case DESTROY_MOM: case SALIR_SALA:
			resultado = MOMDeinit(mom,fd);
			break;
		case LOGIN:
			resultado = MOMLogin(mom,fd,lectura.l);
			break;
		case PEDIR_SALAS:
			salas salas;
			resultado = MOMPedirSalas(mom,fd,salas);
			respuesta.salaP.salas = salas;
			break;
		case ELEGIR_SALA:
			sala sala;
			resultado = MOMPedirAsientosSala(mom,fd,lectura.salaE.salaid,sala);
			respuesta.informacionSala.id = sala.id;
			copiarAsientos(respuesta.informacionSala.estadoAsientos,sala.estadoAsientos);
			break;
		case ACTUALIZAR_ASIENTOS:
			resultado = MOMObtenerActualizacionAsientos(mom,fd,sala);
			respuesta.informacionSala.id = sala.id;
			copiarAsientos(respuesta.informacionSala.estadoAsientos,sala.estadoAsientos);
			break;
		case INTERACCION_ASIENTO:
			resultado = MOMElegirAsiento(mom,fd,lectura.asientoS.asiento);
			break;
		case FINALIZAR_COMPRA:
			resultado = MOMFinalizarCompra(mom,fd);
			break;
	}
	respuesta.resultado = resultado ? RESULTADOOK : RESULTADOERROR;
	return resultado;
}

void MOMDestroy(MOM &mom){
	printf("destroin mom\n");
	std::map<int,MOMClient>::iterator it = mom.clients.begin();
	while(it != mom.clients.end()){
		MOMDeinit(mom,it->first);
		it++;
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

		procesarMensaje(mom,lectura, respuesta);

		if( enviarMensaje(colaEscritura,(void*)&respuesta,sizeof(mensaje)) == -1){
			break;
		}
	}
	MOMDestroy(mom);
	return 0;
}
