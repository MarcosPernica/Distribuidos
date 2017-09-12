#include <stdlib.h>
#include "cinehijo.h"
#include "ipc/cola.h"
#include "common.h"
#include "ipc/senal.h"
#include <vector>

sig_atomic_t hijo_vivo = 1;

void terminarHijo(int sigint){
	hijo_vivo = 0;
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

void administrarCliente(login login){
	std::vector<struct reserva> asientosUsuario;

	if (registrarSenal(SIGINT,terminarHijo) == -1){
		printf("Hijo: error al registrar senal");
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

	bool reservaDeAsiento = false;

	while (hijo_vivo)
	{
		if (recibirMensaje(colaRecepcion,pid,(void*)&msg,sizeof(msg)) == -1)
		{
			//handle error
			printf("Hijo error al recibir mensaje");
			break;
		}
		printf("cinehijo recibio msj\n");
		//Se ve si es una reserva/liberacion de asiento.
		reservaDeAsiento = msg.tipoMensaje == INTERACCION_ASIENTO;

		mensaje respuesta;
		enviarMensaje(colaAdmin,(void*)&msg,sizeof(msg));
		printf("envio a async\n");
		recibirMensaje(colaEnvioAdmin,pid,(void*)&respuesta,sizeof(respuesta));
		printf("recibio de async\n");
		if (enviarMensaje(colaEnvio,(void*)&respuesta,sizeof(respuesta)) == -1)
		{
			//handle error
			printf("Hijo error al enviar mensaje");
			break;
		}
		printf("envio a cliente\n");

		//Se guardan los asientos que el usuario reservo asi despues se liberan en el timeout. 
		if(reservaDeAsiento && respuesta.resultado == RESULTADOOK)
		{
			if(msg.asientoS.estado == ASIENTORESERVADO)
				asientosUsuario.push_back(msg.asientoS);
			else
				quitarAsiento(asientosUsuario, msg.asientoS);

		}
		
		//si se compro se vacia el vector de asientos reservados.
		if(msg.tipoMensaje == FINALIZAR_COMPRA && respuesta.resultado == RESULTADOOK)
			asientosUsuario.clear();
				
	}
	exit(0);
}
