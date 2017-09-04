#include <unistd.h>
#include <sys/wait.h>
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "common.h"
#include "mensajes.h"
#include "clienteAsinc.h"

sig_atomic_t vivo = 0;

void terminar(int sigint){
	vivo = 1;
}

bool entro(mensaje login){
	return true;
}

int main(int argc, char** argv){
	int registrar = registrarSenal(SIGINT,terminar);
	int colaEnvio = obtenerCola(COLA_RECEPCION_CINE);
	int colaRespuesta = obtenerCola(COLA_LOGIN_CINE);
	int pid = getpid();
	mensaje login;
	login.mtype = LOGIN_TYPE;
	login.l.id = pid;

	mensaje respuesta;
	while (vivo == 0){
		//enter params
		if( enviarMensaje(colaEnvio,(void *)&login,sizeof(login)) < 0 )
		{
			break;
		}
		if (recibirMensaje(colaRespuesta,pid,(void*)&respuesta,sizeof(respuesta)) < 0 )
		{
			break;
		}
		if (entro(respuesta)){
			break;
		}
	}

	if (fork() == 0){
		correrAsincronico(getpid());
	}

	while(vivo == 0){
		//hacer request al usuario
		//enviarlo y ver respuesta
		if( enviarMensaje(colaEnvio,(void *)&login,sizeof(login)) < 0 )
		{
			break;
		}
		if (recibirMensaje(colaRespuesta,pid,(void*)&respuesta,sizeof(respuesta)) < 0 )
		{
			break;
		}
	}

	return 0;
}
