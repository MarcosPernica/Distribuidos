/*
 * cineConnectorAsync.cpp
 *
 *  Created on: Oct 10, 2017
 *      Author: tobias
 */
#include "ipc/socket.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "mensajes.h"
#include "common.h"
#include "cineAsyncHandler/cineAsyncHandler.h"
#include <vector>
#include <map>


int obtenerSocket(long id, struct socketMapper* memoriaCompartida, std::map<std::string, int> *sockets)
{
	std::string address = getClientSocket(memoriaCompartida, id);
	if( address != NULL && sockets[address] != NULL ){
		return sockets[address];
	} else {
		//create socket
	}
	return -1;
}

void procesarMensaje(mensaje msg, struct socketMapper* memoriaCompartida, std::map<std::string, int> *sockets )
{
	char buffer[BUFF_SIZE];
	switch(msg.tipoMensaje)
	{
	case AVISOASINCRONO:
		int socket;
		if( (socket = obtenerSocket(msg.mtype, memoriaCompartida, sockets)) != -1 )
		{
			//TODO serializar
			if( escribirSocket( socket, buffer, BUFF_SIZE ) == -1 ){
				printf("No pudo enviar a socket\n");
				break;
			}
		}
		break;
	case SOCKETS_ASYNC_UPDATE:
		//cerrar un socket particular!!!
		break;
	}
}

int main(int argc, char** argv)
{
	std::map<std::string, int> sockets;
	int colaRecibir = obtenerCola(COLA_ASINC_CLIENTE);
	//TODO pedir memoria compartida

	int socket;

	struct socketMapper* memoriaCompartida;
	mensaje msg;
	while( vivo == 0 )
	{

		if( recibirMensaje(colaRecibir,&msg,sizeof(mensaje) ) == -1){
			printf("Error recibiendo mensaje de cola \n");
			break;
		}

		procesarMensaje(msg,memoriaCompartida, &sockets);
	}


	//TODO detroy all
	return 0;
}
