#include "ipc/socket.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "ipc/semaforo.h"
#include "ipc/memoriacompartida.h"
#include "mensajes.h"
#include "paramsParser.h"
#include <vector>
#include <string.h>
#include "common.h"
#include "cineAsyncHandler/cineAsyncHandler.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include "serializador.h"
#include <netinet/in.h>
#include <arpa/inet.h>


#define DEFAULT_PORT 9999

sig_atomic_t estaVivo = 0;

void terminar(int signal)
{
	estaVivo = 1;
}

void procesarClientesSockets(mensaje &msg, struct socketMapper* memoria,int mutex,char* address)
{
	tomarSem(mutex);
	switch(msg.tipoMensaje)
	{
	case LOGIN:
		addClient(memoria,address, strlen(address), msg.mtype);
		break;
	case SALIR_SALA:
		removeClient(memoria,address, strlen(address), msg.mtype);
		break;
	}
	liberarSem(mutex);
}

void handleClient(int socket, struct sockaddr_in cli)
{
	int colaLogin = obtenerCola(COLA_LOGIN_CINE);
	int colaEnvioACine = obtenerCola(COLA_RECEPCION_CINE);
	int colaRecibirDeCine = obtenerCola(COLA_ENVIO_CINE);

	char* address = inet_ntoa(cli.sin_addr);
	printf("IP address: %s\n", address);
	int idMemoriaCompartida;
	int mutex;

	if ((idMemoriaCompartida = cmpMemObtener(sizeof(struct socketMapper), MEMORIA_COMPARTIDA_CINE_ID)) < 0){
		perror("No pudo obtener la memoria compartida");
		exit(1);
	}


	if ((mutex = crearSem(MUTEX_CINE_ID, 1)) == -1 ){
		perror("No pudo obtener el mutex de la memoria compartida");
		exit(1);
	}

	struct socketMapper* memoriaCompartida;
	memoriaCompartida = (struct socketMapper*)cmpMemObtenerMemoria(idMemoriaCompartida);

	tomarSem(mutex);
	addAddress(memoriaCompartida,address,strlen(address),CLIENT_PORT);
	liberarSem(mutex);

	cli.sin_addr;
	char buffer[BUFF_SIZE];
	std::string recibido;
	mensaje aEnviar;
	mensaje aRecibir;
	int lastEnd = -1;
	int totalRead = -1;
	int unusedBufferLength = 0;
	while( estaVivo == 0 )
	{
		if( (totalRead = leerSocketHasta(socket,buffer,BUFF_SIZE,'\0',lastEnd)) == -1 )
		{
			perror("No pudo leer ");
			break;
		}
		//copio stream al string recibido
		recibido = std::string(buffer,lastEnd);
		desserializar( recibido, aEnviar );
		recibido.clear();

		procesarClientesSockets(aEnviar,memoriaCompartida,mutex,address);
		if( enviarMensaje( aEnviar.tipoMensaje == LOGIN ? colaLogin : colaEnvioACine,
				(void*)&aEnviar, sizeof(mensaje) ) == -1)
		{
			perror("No pudo enviar mensaje a la cola ");
			break;
		}

		if ( recibirMensaje(colaRecibirDeCine,(void*)&aRecibir,sizeof(mensaje)) == -1)
		{
			perror("No pudo recibir mensaje de la cola ");
			break;

		}

		serializar(aRecibir, recibido, false);

		if( escribirSocketEntero(socket, (char*)recibido.c_str(), recibido.size()) == -1){
			perror("No pudo escribir en socket ");
			break;
		}
		recibido.clear();
	}
	tomarSem(mutex);
	removeAddress(memoriaCompartida,address,strlen(address));
	liberarSem(mutex);

	close(socket);

	//ENVIAR MENSAJE CERRAR SOCKET
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
		printf("No port specified taking default port %i\n",port);
	} else if( parsePort(argv[1], &port) ){
		printf("Puerto a usar: %i\n",port);
	}

	int socket_server = crearSocketServer(port,20);
	if( socket_server == -1){
		printf("No pudo crear socket servidor\n");
		exit(1);
	}

	struct sockaddr_in cli;
	socklen_t clilen = sizeof(cli);
	int childpid;
	while( estaVivo == 0 )
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
		waitpid(hijos[0],NULL,0);
		hijos.erase(hijos.begin());
	}
	close(socket_server);
	return 0;
}
