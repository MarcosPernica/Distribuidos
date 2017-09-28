#include <unistd.h>
#include <sys/wait.h>
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "common.h"
#include "mensajes.h"
#include "cinehijo.h"
#include "administrador.h"
#include "baseDeDatos/baseDeDatos.h"
#include "errno.h"

sig_atomic_t vivo = 0;

void terminar(int sigint){
	vivo = 1;
	//mandar senal a hijos?
}

bool loginUser(struct user) {
	return true;
}

int main(int argc,char** argv)
{
	//initalize
	int hijos = 0;

	//para finalizar
	int senalRegistrada = registrarSenal(SIGINT,terminar);
	if (senalRegistrada < 0){
		printf("Error en registrar senial %d \n",senalRegistrada);
		return 1;
	}

	//para cola de recibo
	int colaLogin = obtenerCola(COLA_LOGIN_CINE);
	if (colaLogin < 0){
		printf("Error en cola recepcion %d \n",colaLogin);
		return 1;
	}

	//para cola de respuetsa
	int colaRespuesta = obtenerCola(COLA_ENVIO_CINE);
	if (colaRespuesta < 0){
		printf("Error en cola envio %d \n",colaRespuesta);
		return 1;
	}

	if (fork() == 0){
		correrAdministrador();
	}

	//correr
	while (vivo == 0){
		mensaje leido;
		if (recibirMensaje(colaLogin,LOGIN_TYPE,(void *)&leido,sizeof(leido)) == -1 )
		{
			printf("Error al recibir login \n");
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
		loginResponse.resultado = RESULTADOOK;
		if ( enviarMensaje(colaRespuesta,(void*)&loginResponse,sizeof(loginResponse)) == -1 )
		{
			perror("Error: ");
			printf("Error al enviar datos de login \n");
			break;
		}
	}

	//esperar hijos
	for (int i = 0; i < hijos + 1; i++){
		wait(NULL);
	}
	//destory all
}
