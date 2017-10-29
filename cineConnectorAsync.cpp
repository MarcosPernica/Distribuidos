/*
 * cineConnectorAsync.cpp
 *
 *  Created on: Oct 10, 2017
 *      Author: tobias
 */
#include "ipc/socket.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "ipc/memoriacompartida.h"
#include "mensajes.h"
#include "common.h"
#include "cineAsyncHandler/cineAsyncHandler.h"
#include "serializador.h"
#include "ipc/semaforo.h"
#include <vector>
#include <map>
#include <unistd.h>

sig_atomic_t estaVivo = 0;

void terminar(int signal)
{
	estaVivo = 1;
}

/** Cierra todos los sockets a los que se conecto */
void cerrarTodosLosSockets(std::map<std::string, int> &sockets){
	for (std::map<std::string, int>::iterator it=sockets.begin(); it!=sockets.end(); ++it){
		close(it->second);
	}
	sockets.clear();
}

/** Cierra el socket para esa direccion si es que existe*/
void cerrarSocket(std::string address, std::map<std::string, int> &sockets)
{
	std::map<std::string,int>::iterator it = sockets.find(address);
	if( it != sockets.end() )
	{
		close( sockets[address] );
	}
	sockets.erase(address);
}

/* Obtiene el socket para el cliente pedido, si no existia lo crea
 * Devuelve -1 si el socket no se pudo crear*/
int obtenerSocket(long id, struct socketMapper* memoriaCompartida,
		std::map<std::string, int> &sockets,
		int mutex)
{
	int port;

	tomarSem(mutex);
	std::string address = getClientAddress(memoriaCompartida, (int)id, &port);
	liberarSem(mutex);

	std::map<std::string,int>::iterator it = sockets.find(address);
	if( address != "" && it != sockets.end() ){
		return sockets[address];
	} else {
		int socket;
		if( (socket = crearSocketCliente(address, port)) != -1 ){
			sockets[address] = socket;
			return socket;
		}
	}
	return -1;
}

/** Procesa el mensaje de la cola, si es el aviso lo envia por el socket,
 * sino cierra el socket que deberia cerrar */
void procesarMensaje(mensaje msg,
		struct socketMapper* memoriaCompartida,
		std::map<std::string, int> &sockets,
		int mutex )
{
	char buffer[BUFF_SIZE];
	switch(msg.tipoMensaje)
	{
	case AVISOASINCRONO:
		int socket;
		if( (socket = obtenerSocket(msg.mtype, memoriaCompartida, sockets, mutex)) != -1 )
		{
			std::string aEnviar;
			serializar(msg,aEnviar);
			if( escribirSocketEntero( socket, (char*)aEnviar.c_str(), aEnviar.size() + 1 ) == -1 ){
				printf("No pudo enviar a socket\n");
				break;
			}
		}
		break;
	case SOCKETS_ASYNC_UPDATE:
		std::string address = std::string(msg.address);
		cerrarSocket(address,sockets);
		break;
	}
}

int main(int argc, char** argv)
{
	if( registrarSenal(SIGINT,terminar) < 0){
		perror("No pudo registrar senial");
		exit(1);
	}

	std::map<std::string, int> sockets;
	int colaRecibir = obtenerCola(COLA_ASINC_CLIENTE);

	int idMemoriaCompartida;
	int mutex;

	if ((idMemoriaCompartida = cmpMemObtener(sizeof(struct socketMapper), MEMORIA_COMPARTIDA_CINE_ID)) < 0)
	{
		perror("No pudo obtener memoria compartida ");
		return -1;
	}


	if ((mutex = obtenerSem(MUTEX_CINE_ID)) == -1 ){
		perror("No pudo obtener mutex ");
		return -1;
	}


	int socket;
	struct socketMapper* memoriaCompartida;
	memoriaCompartida = (struct socketMapper*)cmpMemObtenerMemoria(idMemoriaCompartida);
	mensaje msg;
	while( estaVivo == 0 )
	{

		printf("Recibio mensaje asincronico \n");
		if( recibirMensaje(colaRecibir,&msg,sizeof(mensaje) ) == -1){
			printf("Error recibiendo mensaje de cola \n");
			break;
		}

		procesarMensaje(msg,memoriaCompartida, sockets, mutex);
	}

	cmpMemDesvincular(memoriaCompartida);
	cerrarTodosLosSockets(sockets);
	return 0;
}
