#include "apiMensajes.h"
#include "../common.h"
#include "../mensajes.h" 
#include "../ipc/cola.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

static int consultarMOM(mensaje &consulta,int id, mensaje &respuesta)
{
	int colaEnvio = obtenerCola(COLA_CLIENT_MOM);
	int colaRecepcion = obtenerCola(COLA_MOM_CLIENT);

	if ( enviarMensaje(colaEnvio,(void *)&consulta,sizeof(mensaje)) == -1 ){
		printf("Error enviando mensaje al mom\n");
		return -1;
	}

	if (recibirMensaje(colaRecepcion, id, (void *)&respuesta,sizeof(mensaje)) == -1){
		printf("Error enviando mensaje al mom\n");
		return -1;
	}
	return 0;
}

int APIInit()
{
	mensaje init;
	mensaje respuesta;
	init.mtype = getpid();
	init.tipoMensaje = INITMOM;
	init.initmom.fd = -1;
	init.initmom.pid = getpid();
	consultarMOM(init,getpid(),respuesta);
	return respuesta.initmom.fd;
}


bool APILogin(int fd, struct login &login)
{
	mensaje init;
	mensaje respuesta;
	init.fd = fd;
	init.mtype = getpid();
	init.tipoMensaje = LOGIN;
	init.l = login;
	consultarMOM(init,getpid(),respuesta);
	return respuesta.resultado >= 0 ;
}


bool APIPedirSalas(int fd, struct salas &salas)
{
	mensaje init;
	mensaje respuesta;
	init.fd = fd;
	init.mtype = getpid();
	init.tipoMensaje = PEDIR_SALAS;
	consultarMOM(init,getpid(),respuesta);
	if( respuesta.resultado < 0 ){
		return false;
	}
	salas = respuesta.salaP.salas;
	return true;
}

bool APIPedirAsientosSala(int fd, int idSala,struct sala &estadoAsientos)
{
	mensaje init;
	mensaje respuesta;
	init.fd = fd;
	init.mtype = getpid();
	init.tipoMensaje = ELEGIR_SALA;
	init.salaE.salaid = idSala;
	consultarMOM(init,getpid(),respuesta);
	if( respuesta.resultado < 0 ){
		return false;
	}
	estadoAsientos.id = respuesta.informacionSala.id;
	copiarAsientos(estadoAsientos.estadoAsientos, respuesta.informacionSala.estadoAsientos);
	return true;
}

//Obtiene el estado de los asientos.
bool APIObtenerActualizacionAsientos(int fd, struct sala &estadoAsientos)
{
	mensaje init;
	mensaje respuesta;
	init.fd = fd;
	init.mtype = getpid();
	init.tipoMensaje = ACTUALIZAR_ASIENTOS;
	consultarMOM(init,getpid(),respuesta);
	if( respuesta.resultado < 0 ){
		return false;
	}
	estadoAsientos.id = respuesta.informacionSala.id;
	copiarAsientos(estadoAsientos.estadoAsientos,respuesta.informacionSala.estadoAsientos);
	return true;
}

bool APIElegirAsiento(int fd,struct asiento &asiento)
{
	mensaje init;
	mensaje respuesta;
	init.fd = fd;
	init.mtype = getpid();
	init.tipoMensaje = INTERACCION_ASIENTO;
	init.asientoS.asiento = asiento;
	consultarMOM(init,getpid(),respuesta);
	if( respuesta.resultado < 0 )
	{
		return false;
	}
	return true;
}

bool APIFinalizarCompra(int fd)
{

	mensaje init;
	mensaje respuesta;
	init.fd = fd;
	init.mtype = getpid();
	init.tipoMensaje = FINALIZAR_COMPRA;
	consultarMOM(init,getpid(),respuesta);
	if( respuesta.resultado < 0 )
	{
		return false;
	}
	return true;
}

bool APIDeinit(int fd)
{
	mensaje init;
	mensaje respuesta;
	init.fd = fd;
	init.mtype = getpid();
	init.tipoMensaje = DESTROY_MOM;
	consultarMOM(init,getpid(),respuesta);
	if( respuesta.resultado < 0 )
	{
		return false;
	}
	return true;
}
