#include <unistd.h>
#include <sys/wait.h>
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "common.h"
#include "mensajes.h"
#include "cinehijo.h"
#include "administrador.h"
#include "baseDeDatos/baseDeDatos.h"

sig_atomic_t vivo = 0;

void terminar(int sigint){
	vivo = 1;
	//mandar senal a hijos?
}

int main(int argc,char** argv)
{
	//initalize
	int hijos = 0;

	//para finalizar
	int senalRegistrada = registrarSenal(SIGINT,terminar);
	if (senalRegistrada < 0){
		printf("Error en registrar senial %d \n",senalRegistrada);
	}

	//para cola de recibo
	int colaLogin = obtenerCola(COLA_LOGIN_CINE);
	if (colaLogin < 0){
		printf("Error en cola recepcion %d \n",colaLogin);
	}

	//para cola de respuetsa
	int colaRespuesta = obtenerCola(COLA_ENVIO_CINE);
	if (colaRespuesta < 0){
		printf("Error en cola envio %d \n",colaRespuesta);
	}

	if (fork() == 0){
		correrAdministrador();
	}

	//correr
	while (vivo == 0){
		mensaje leido;
		if (recibirMensaje(colaLogin,LOGIN_TYPE,(void *)&leido,sizeof(leido)) == -1 )
		{
			//TODO handle erro
			printf("Error al recibir datos \n");
			break;
		}

		loginResponse response;
		if (loginUser(leido.l.user))
		{
			if (fork() == 0){
				administrarCliente(leido.l);
			}
			hijos++;
			response.success = 1;
		} else {
			response.success = 0;
		}

		mensaje loginResponse;
		loginResponse.mtype = leido.l.id;
		loginResponse.tipoMensaje = LOGIN_RESPONSE;
		loginResponse.lresponse = response;
		if (enviarMensaje(colaLogin,(void*)&loginResponse,sizeof(loginResponse)) == -1){
			//TODO handle error
			printf("Error al enviar datos \n");
			break;
		}
	}

	//esperar hijos
	for (int i = 0; i < hijos + 1; i++){
		wait(NULL);
	}
	//destory all
}
