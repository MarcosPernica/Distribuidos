#include <stdlib.h>
#include "cinehijo.h"
#include "ipc/cola.h"
#include "common.h"
#include "ipc/senal.h"
#include <vector>
#include <unistd.h>
#include "errno.h"
#include "timeout.h"

sig_atomic_t hijo_vivo = 1;
sig_atomic_t alarma = 0;

void terminarHijo(int sigint){
	hijo_vivo = 0;
}

void alarmHandler(int sigint){
	alarma = 1;
}

void quitarAsiento(std::vector<struct reserva> &asientosUsuario, struct reserva asiento)
{
	for(unsigned int i = 0; i < asientosUsuario.size(); i++)
		if(asientosUsuario[i].asiento == asiento.asiento)
		{
			asientosUsuario.erase(asientosUsuario.begin()+i);
			break;
		}
}

void printColaError(){
	if (errno != EINTR ){
		printf("Hijo error en cola de mensajes\n");
	} else {
		printf("Exiting..\n");
	}
}

void administrarCliente(login login){
	std::vector<struct reserva> asientosUsuario;

	if( registrarSenal(SIGINT,terminarHijo) == -1){
		printf("Hijo: error al registrar senal");
		exit(1);
	}
	if( registrarSenal(SIGALRM,alarmHandler) == -1 ){
		printf("Hijo: error al registrar senal timeout");
		exit(1);
	}
	int pid = login.id;
	mensaje msg;
	int colaRecepcion = obtenerCola(COLA_RECEPCION_CINE);
	if (colaRecepcion == -1){
		printf("Hijo error al obtener cola recepcion");
		exit(1);
	}

	int colaEnvio = obtenerCola(COLA_ENVIO_CINE);
	if (colaEnvio == -1){
			printf("Hijo error al obtener cola envio");
			exit(1);
		}

	int colaAdmin = obtenerCola(COLA_RECEPCION_ADMIN);
	int colaEnvioAdmin = obtenerCola(COLA_ENVIO_ADMIN);

	bool usuarioSale = false;
	bool timeOut = false;

	while (hijo_vivo && !usuarioSale)
	{
		ponerAlarma(TIME_OUT_USER);
		while ( recibirMensaje(colaRecepcion,pid,(void*)&msg,sizeof(msg)) == -1 )
		{
			if( ocurrioAlarma(alarma) ){
				printf("Hubo timeout para %d\n",pid);
				timeOut = true;
				mensaje aux;
				aux.mtype = pid;
				aux.tipoMensaje = TIMEOUT;
				aux.idUsuario = pid;
				msg = aux;
			} else {
				printColaError();
			}
			break;
		}
		cancelarAlarma();

		//Se ve si el usuario esta saliendo
		usuarioSale = msg.tipoMensaje == SALIR_SALA;

		mensaje respuesta;
		if ( enviarMensaje(colaAdmin,(void*)&msg,sizeof(msg)) == 1 ){
			printColaError();
			break;
		}
		if ( recibirMensaje(colaEnvioAdmin,pid,(void*)&respuesta,sizeof(respuesta)) == -1 ){
			printColaError();
			break;
		}

		if ( timeOut ){
			break;
		}

		if (enviarMensaje(colaEnvio,(void*)&respuesta,sizeof(respuesta)) == -1)
		{
			printColaError();
			break;
		}

	}
	exit(0);
}
