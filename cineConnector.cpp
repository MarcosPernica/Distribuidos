#include "ipc/socket.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "mensajes.h"
#include <vector>

#define DEFAULT_PORT 9999

sig_atomic_t estaVivo = 0;

void terminar(int signal)
{
	estaVivo = 1;
}

void handleClient(int socket, struct sockaddr_in cli){
	int colaEnvioACine = obtenerCola(1);
	int colaRecibirDeCine = obtenerCola(2);
	//TODO buff size
	int BUFF_SIZE = 1024;
	char buffer[1024];

	while( estaVivo == 0 )
	{
		if( leerSocket(socket,buffer,BUFF_SIZE) == -1 ){
			perror("No pudo leer ");
			break;
		}
		//deserializar
		//enviarMensaje();
		//recibirCola();
		//serializar
		if( escribirSocket(socket, buffer, BUFF_SIZE) == -1){
			perror("No pudo escribir en socket ");
			break;
		}
	}
	close(socket);
}

int main(int argc, char** argv)
{
	if( registrarSenal(SIGINT,terminar) < 0){
		perror("No pudo registrar senial");
		exit(1);
	}

	std::vector<int> hijos;
	int port = DEFAULT_PORT;
	if( argc != 2 ){
		printf("No port specified taking default port %d\n",DEFAULT_PORT);
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
			handleClient(client, cli);
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
		wait(NULL);
		hijos.pop_back();
	}
	close(server_socket);
	return 0;
}
