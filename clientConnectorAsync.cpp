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

sig_atomic_t vivo = 0;

void terminar(int signal)
{
	vivo = 1;
}


void handleAsync(int socketfd, sockaddr_in cli){
	char buffer[1024];
	int cola = obtenerCola(1); //tiene que ser la asincronica
	while( leerSocket(socketfd,buffer,1024) != -1 && vivo == 0){
		mensaje mensaje;
		//deserialiar mensaje
		enviarMensaje(cola,(void*)&mensaje,sizeof(mensaje));
	}
}

int main(int argc, char** argv)
{
	if( registrarSenal(SIGINT,terminar) < 0){
		perror("No pudo registrar senial");
		exit(1);
	}

	std::vector<int> hijos;
	int port = 8000;
	if( argc != 2 ){
		printf("No port specified taking default port %d\n",port);
	}
	//argv 1 : port

	int socket_server = crearSocketServer(port);
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
			close(server_socket);
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
	close(server_socket);
	return 0;
}

