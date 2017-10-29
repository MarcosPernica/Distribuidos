/*
 * clientConnectorAsync.cpp
 *
 *  Created on: Oct 10, 2017
 *      Author: tobias
 */
#include "ipc/socket.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "mensajes.h"
#include <vector>
#include "paramsParser.h"
#include "common.h"
#include "serializador.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "serializador.h"
#include <netinet/in.h>
#include <arpa/inet.h>


sig_atomic_t vivo = 0;

void terminar(int signal)
{
	vivo = 1;
}


void handleAsync(int socketfd, sockaddr_in cli){
	char buffer[BUFF_SIZE];
	int cola = obtenerCola(COLA_ASINC_CLIENTE);

	mensaje mensaje;
	std::string recibido;

	int endLine = -1;
	int totalRead = -1;
	int unusedBufferLength;

	while( vivo == 0){

		//si hay mas mensajes en lo leido
		if( totalRead > endLine + 1 )
		{
			//copio elementos al principio
			unusedBufferLength = totalRead - endLine + 1;
			for( int i = 0; i < unusedBufferLength; i ++)
			{
				buffer[i] = buffer[endLine + 1 + i];
			}
		} else
		{
			unusedBufferLength = 0;
		}


		endLine = nextIndexOf('\0', buffer, 0, unusedBufferLength);
		// si no hay otro mensaje en el buffer o hay un 0 en la basura(falso mensaje)
		// lee del socket lo restante
		if ( endLine == -1 || endLine + 1 > unusedBufferLength )
		{
			if( (totalRead = leerSocketHasta(socketfd,
					buffer + unusedBufferLength,
					BUFF_SIZE - unusedBufferLength,'\0',endLine)) == -1)
			{
				perror("Error leyendo socket: ");
				break;
			}
		}

		recibido = std::string(buffer, endLine);
		desserializar(recibido, mensaje);
		enviarMensaje(cola,(void*)&mensaje,sizeof(mensaje));
		recibido.clear();
	}
}

int main(int argc, char** argv)
{
	if( registrarSenal(SIGINT,terminar) < 0){
		perror("No pudo registrar senial");
		exit(1);
	}

	std::vector<int> hijos;
	int port = CLIENT_PORT;
	printf("Port for client async : %d \n", port);

	int socket_server = crearSocketServer(port,10);
	if( socket_server == -1){
		printf("No pudo crear socket servidor\n");
		exit(1);
	}

	struct sockaddr_in cli;
	socklen_t clilen = sizeof(cli);
	int childpid;
	while( vivo == 0 )
	{
		int client = accept(socket_server, (struct sockaddr *) &cli, &clilen );
		if( client == -1 )
		{
			perror("Accept error\n");
			break;
		}

		if( (childpid = fork()) == 0 )
		{
			close(socket_server);
			handleAsync(client, cli);
			close(client);
			exit(0);
		}
		hijos.push_back(childpid);
		close(client);
	}

	//kill children
	std::vector<int>::iterator it = hijos.begin();
	for(; it != hijos.end(); it++){
		kill( *it , SIGINT);
	}

	//wait children
	while( hijos.size() > 0)
	{
		waitpid(hijos[0],NULL,0);
		hijos.pop_back();
	}
	close(socket_server);
	return 0;
}

