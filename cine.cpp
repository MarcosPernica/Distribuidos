#include <unistd.h>
#include <sys/wait.h>
#include "ipc/cola.h"
#include "common.h"
#include "mensajes.h"
#include "cinehijo.h"
#include "baseDeDatos/baseDeDatos.h"

int servidorEstaVivo(){
	//TODO exit with signal
	return 1;
}

int main(int argc,char** argv)
{
	//initalize
	int hijos = 0;
	int colaLogin = obtenerCola(COLA_LOGIN_CINE);
	while (servidorEstaVivo()){
		mensaje leido;
		if (recibirMensaje(colaLogin,LOGIN_TYPE,(void *)&leido,sizeof(leido)) == -1 )
		{
			//TODO handle mensaje
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
		enviarMensaje(colaLogin,(void*)&loginResponse,sizeof(loginResponse));
	}
	for (int i = 0; i < hijos; i++){
		wait(NULL);
	}
	//destory all
}
