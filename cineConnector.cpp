#include "ipc/socket.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "ipc/semaforo.h"
#include "ipc/memoriacompartida.h"
#include "mensajes.h"
#include "paramsParser.h"
#include "timeout.h"
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

/** Modifica memoria compartida para indicarle al asincronico que clientes hay donde */
void procesarClientesSockets(mensaje &msg, struct socketMapper* memoria,int mutex,char* address)
{
	tomarSem(mutex);
	switch(msg.tipoMensaje)
	{
	case LOGIN:
		printf("agrego cliente %i a %s\n",msg.l.id, address);
		addClient(memoria,address, strlen(address), msg.l.id);
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
		close(socket);
		exit(1);
	}


	if ( ( mutex = obtenerSem(MUTEX_CINE_ID) ) == -1 ){
		perror("No pudo obtener el mutex de la memoria compartida");
		close(socket);
		exit(1);
	}

	struct socketMapper* memoriaCompartida;
	memoriaCompartida = (struct socketMapper*)cmpMemObtenerMemoria(idMemoriaCompartida);

	tomarSem(mutex);
	addAddress(memoriaCompartida,address,strlen(address),CLIENT_PORT);
	liberarSem(mutex);


	char buffer[BUFF_SIZE];
	std::string recibido;
	mensaje aEnviar;
	mensaje aRecibir;
	int lastEnd = -1;
	int totalRead = -1;
	int unusedBufferLength = 0;
	int idToWait = 0;
	while( estaVivo == 0 )
	{
		printf("%i. Esperando lectura de socket\n", getpid());
		if( (totalRead = leerSocketHasta(socket,buffer,BUFF_SIZE,'\0',lastEnd)) == -1 )
		{
			perror("No pudo leer ");
			break;
		}

		//copio stream al string recibido
		recibido = std::string(buffer,lastEnd);
		printf("Por deserializar %s\n", recibido.c_str());
		desserializar( recibido, aEnviar );
		printf("%i. Deserializo %s\n", getpid(), recibido.c_str());
		recibido.clear();

		procesarClientesSockets(aEnviar,memoriaCompartida,mutex,address);

		printf("%i. Envio mensaje a cola\n", getpid());
		if( enviarMensaje( aEnviar.tipoMensaje == LOGIN ? colaLogin : colaEnvioACine,
				(void*)&aEnviar, sizeof(mensaje) ) == -1)
		{
			perror("No pudo enviar mensaje a la cola ");
			break;
		}

		printf("%i. Recibir respuesta de cola\n", getpid());
		idToWait = aEnviar.tipoMensaje == LOGIN ? aEnviar.l.id : aEnviar.mtype;
		if ( recibirMensaje(colaRecibirDeCine,idToWait,(void*)&aRecibir,sizeof(mensaje)) == -1)
		{
			perror("No pudo recibir mensaje de la cola ");
			break;

		}

		serializar(aRecibir, recibido, false);

		printf("%i. Escribir respuesta en socket %s \n", getpid(), recibido.c_str());
		if( escribirSocketEntero(socket, (char*)recibido.c_str(), recibido.size() + 1) == -1){
			perror("No pudo escribir en socket ");
			break;
		}
		recibido.clear();
	}

	tomarSem(mutex);
	removeAddress(memoriaCompartida,address,strlen(address));
	liberarSem(mutex);

	close(socket);
	printf("Cerro socket a cliente\n");

	//envio mensaje a asincrono para que desconecte el cliente
	int colaAsync = obtenerCola(COLA_ASINC_CLIENTE);
	mensaje cerrarSocket;
	cerrarSocket.tipoMensaje = SOCKETS_ASYNC_UPDATE;
	memcpy(cerrarSocket.address,address,strlen(address));
	enviarMensaje(colaAsync,(void*)&cerrarSocket,sizeof(mensaje));
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
