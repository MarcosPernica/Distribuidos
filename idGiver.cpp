/*
 * idGiver.cpp
 *
 *  Created on: Oct 3, 2017
 *      Author: tobias
 */

#include "ipc/socket.h"
#include "ipc/senal.h"
#include <string>
#include "serializador.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>

sig_atomic_t vivo = 0;
void terminar(int sigint){
	vivo = 1;
}

int main(int argc, char**argv){
	if( registrarSenal(SIGINT,terminar) < 0){
		perror("No pudo registrar senial");
		exit(1);
	}

	int port = 27772;
	int id = 1;
	if( argc != 2 ){
		printf("No port specified taking default port %d\n",port);
	}
	else
	{
		std::string puertoArg(argv[1]);
		desserializar(puertoArg, port);
	}
	
	

	int socket_server = crearSocketServer(port);
	if( socket_server == -1){
		printf("No pudo crear socket servidor\n");
		exit(1);
	}

	struct sockaddr_in cli;
	socklen_t clilen = sizeof(cli);
	int childpid;
	std::string numero;

	while( vivo == 0 )
	{
		int client = accept(socket_server, (struct sockaddr *) &cli, &clilen );
		if( client == -1 )
		{
			perror("Accept error\n");
			break;
		}

		serializar(id,numero);

		if( escribirSocket(client,numero.c_str(), numero.length()+1) == -1){
			perror("error en envio de numero");
			break;
		}

		close(client);

		id++;
	}
}


