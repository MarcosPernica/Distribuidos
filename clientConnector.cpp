/*
 * clientConnector.cpp
 *
 *  Created on: Oct 3, 2017
 *      Author: tobias
 */
#include "ipc/socket.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "serializador.h"
#include "paramsParser.h"
#include "mensajes.h"
#include "common.h"
#include <unistd.h>

sig_atomic_t vivo = 0;
void terminar(int sig){
	vivo = 1;
}

int main(int argc, char** argv)
{
	char host[16] = "127.0.0.1";
	int port = DEFAULT_CINE_PORT;

	if( argc >= 2 )
	{
		if( !parseIp(argv[1],host) )
		{
			printf("Ip invalido: %s\n",argv[1]);
		}
	}

	if( argc == 3 ){

		if( !parsePort(argv[2],&port) )
		{
			printf("Puerto invalido: %s\n",argv[2]);
		}
	}
	printf("Conectando a ip : %s y puerto: %d\n", host,port);

	int colaRecibir = obtenerCola(COLA_RECEPCION_CINE);
	int colaEnvio = obtenerCola(COLA_ENVIO_CINE);

	int socket = crearSocketCliente(host, port);
	if( socket == -1 )
	{
		perror("No pudo conectarse");
		exit(1);
	}

	char buffer[BUFF_SIZE];
	mensaje recibido;
	mensaje aEnviar;
	bool errored = false;
	std::string respuesta;
	while( vivo == 0 )
	{
		if( recibirMensaje(colaRecibir,(void*)&recibido,sizeof(mensaje)) == -1)
		{
			printf("No pudo recibir de cola\n");
			break;
		}
		serializar(recibido,respuesta);

		if( escribirSocketEntero( socket, (char*)respuesta.c_str(), (int)respuesta.size() ) == -1 ){
			printf("No pudo enviar a socket\n");
			aEnviar = recibido;
			aEnviar.resultado = RESULTADOERROR;
			enviarMensaje(colaEnvio,(void*)&aEnviar,sizeof(mensaje));
			break;
		}
		respuesta.clear();

		if( leerSocketEntero(socket, buffer, BUFF_SIZE) == -1 ){
			printf("No pudo leer del socket\n");
			aEnviar = recibido;
			aEnviar.resultado = RESULTADOERROR;
			enviarMensaje(colaEnvio,(void*)&aEnviar,sizeof(mensaje));
			break;
		}

		desserializar(respuesta,aEnviar,false);
		respuesta.clear();

		if( enviarMensaje(colaEnvio,(void*)&aEnviar,sizeof(mensaje)) == -1){
			printf("No pudo enviar a cola\n");
			break;
		}

	}
	close(socket);

	return 0;
}
