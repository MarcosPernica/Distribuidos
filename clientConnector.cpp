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
#include <errno.h>
#include <time.h>
#include "timeout.h"

sig_atomic_t vivo = 0;

void handleAlarm(int sig){

}

void terminar(int sig){
	vivo = 1;
}

int main(int argc, char** argv)
{
	if( registrarSenal(SIGINT,terminar) < 0){
		perror("No pudo registrar senial ctrl c");
		exit(1);
	}

	if( registrarSenal(SIGALRM,handleAlarm) < 0){
		perror("No pudo registrar senial de alarma");
		exit(1);
	}
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
	int colaLogin = obtenerCola(COLA_LOGIN_CINE);

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
	int colaArecibir = colaLogin;
	int totalRead = -1;
	int endLine = -1;
	struct timespec nanoTime;
	nanoTime.tv_sec = 0;
	nanoTime.tv_nsec = 1000;
	while( vivo == 0 )
	{
		printf("Esperando mesaje de cola\n");
		while( recibirMensajeAsinc(colaArecibir,0,(void*)&recibido,sizeof(mensaje)) == -1)
		{
			if( errno == ENOMSG ){
				colaArecibir = colaArecibir == colaLogin ? colaRecibir : colaLogin;
				if ( nanosleep(&nanoTime,NULL) == -1){
					errored = true;
					break;
				}
				continue;
			}
			errored = true;
			printf("No pudo recibir de cola\n");
			break;
		}

		if( errored ){
			break;
		}

		printf("Leyo mensaje %i \n", recibido.tipoMensaje);
		serializar(recibido,respuesta);

		printf("Escribiendo a socket %i\n",respuesta.size() + 1);
		printf("Escribiendo a socket %s\n",respuesta);
		if( escribirSocketEntero( socket, (char*)respuesta.c_str(), respuesta.size() + 1 ) == -1 ){
			printf("No pudo enviar a socket\n");
			aEnviar = recibido;
			aEnviar.resultado = RESULTADOERROR;
			enviarMensaje(colaEnvio,(void*)&aEnviar,sizeof(mensaje));
			break;
		}
		respuesta.clear();

		printf("Esperando respuesta del socket\n");
		ponerAlarma(TIME_OUT_CONNECTION);
		if( ( totalRead = leerSocketHasta(socket,buffer,BUFF_SIZE,'\0',endLine)) == -1 ){
			bool alarm = errno == EINTR && vivo == 0;
			perror("No pudo leer del socket: ");
			aEnviar = recibido;
			aEnviar.resultado = RESULTADOERROR;
			enviarMensaje(colaEnvio,(void*)&aEnviar,sizeof(mensaje));
			perror("Envio mensaje\n ");
			if( alarm ) continue;
			else break;
		}
		cancelarAlarma();

		if( endLine != -1){
			respuesta = std::string(buffer,endLine);
			desserializar(respuesta,aEnviar,false);
			respuesta.clear();
		} else {
			aEnviar = recibido;
			aEnviar.resultado = RESULTADOERROR;
		}

		printf("Enviando mensaje de respuesta a la cola\n");
		if( enviarMensaje(colaEnvio,(void*)&aEnviar,sizeof(mensaje)) == -1){
			printf("No pudo enviar a cola\n");
			break;
		}

	}
	close(socket);

	return 0;
}
