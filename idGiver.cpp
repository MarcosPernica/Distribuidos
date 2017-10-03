/*
 * idGiver.cpp
 *
 *  Created on: Oct 3, 2017
 *      Author: tobias
 */

#include "ipc/socket.h"
#include "ipc/senal.h"

sig_atomic_t vivo = 0;
void terminar(int sigint){
	vivo = 1;
}

void serializarInt(int valor, char* buff){

}

int main(int argc, char**argv){
	if( registrarSenal(SIGINT,terminar) < 0){
		perror("No pudo registrar senial");
		exit(1);
	}

	int id = 1;

	int port = 27772;
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
	char num[4];

	while( vivo == 0 )
	{
		int client = accept(socket_server, (struct sockaddr *) &cli, &clilen );
		if( client == -1 )
		{
			perror("Accept error\n");
			break;
		}

		serializarInt(id,num);

		if( escribirSocket(client,num,4) == -1){
			perror("error en envio de numero");
			break;
		}

		close(client);

		id++;
		//TODO persist id
	}
}


