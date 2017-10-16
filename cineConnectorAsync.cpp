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
#include <vector>

int main(int argc, char** argv)
{
	char host[16] = "127.0.0.1";
	int port = 8000;
	//argv 1: host
	//argv 2 : port TODO read from args

	//obtener colas a leer,escribir
	int colaRecibir = obtenerCola(1);

	int socket = crearSocketCliente(host, port);
	if( socket == -1 )
	{
		perror("No pudo conectarse");
		exit(1);
	}

	int BUFF_SIZE = 1024;
	char buffer[BUFF_SIZE];
	while( vivo == 0 )
	{
		//leerCola
		//serializar
		if( escribirSocket( socket, buffer, BUFF_SIZE ) == -1 ){
			printf("No pudo enviar a socket\n");
			//enviar respuesta por cola
			break;
		}

		if( leerSocket(socket, buffer, BUFF_SIZE) == -1 ){
			printf("No pudo leer del socket\n");
			//enviar respuesta por cola
			break;
		}
		//deserializar
		//enviar a cola

	}
	close(socket);

	return 0;
}
