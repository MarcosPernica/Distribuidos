#include "apiMensajes.h"
#include "../common.h"
#include "../mensajes.h" 
#include "../ipc/senal.h" 
#include "../clienteAsinc.h"
#include "../timeout.h"
#include "../ipc/memoriacompartida.h"
#include "../ipc/semaforo.h"
#include "../ipc/cola.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

sig_atomic_t alarma = 0;

static void alarmHandler(int sigint){
	alarma = 1;
}

static int consultarCine(int colaEnvio, int colaRecepcion, mensaje &consulta, int pid, mensaje &respuesta)
{

	if ( enviarMensaje(colaEnvio,(void *)&consulta,sizeof(mensaje)) == -1 ){
		printf("Error enviando mensaje al cine\n");
		return -1;
	}

	return 0;

}

bool APIInit(struct ApiMensajes &api, int ID)
{
	//Obtiene la cola de identificacion con el cine
	api.colaLogin = obtenerCola(COLA_LOGIN_CINE);

	//Obtiene las colas de comunicaciones con el cine hijo
	api.colaEnvio = obtenerCola(COLA_RECEPCION_CINE);
	api.colaRecepcion = obtenerCola(COLA_ENVIO_CINE);

	//Crea la memoria compartida y el mutex para accederla.
	if ((api.idMemoriaCompartida = cmpMemCrear(sizeof(struct sala), getpid())) < 0)
		return false;

	if ((api.mutexMemoriaCompartida = crearSem(getpid(), 1)) == -1 )
		return false;

	api.memoriaCompartida = (struct sala*)cmpMemObtenerMemoria(api.idMemoriaCompartida);

	//Lanza el proceso asincrono que va a tomar los datos de actualizacion.
	api.pidAsincrono = fork();
	if ( api.pidAsincrono == 0){
		correrAsincronico(ID);
		exit(0);
	}

	//Registra el timer.
	registrarSenal(SIGALRM,alarmHandler);


	//Guarda el ID de comunicacion.
	api.pid = ID;

	api.estaInicializado = true;

	return true;
}


bool APILogin(struct ApiMensajes &api, mensaje &login)
{
	if(!api.estaInicializado)
		return false;

	login.mtype = LOGIN_TYPE;
	login.l.id = api.pid;
	mensaje respuesta;

	if ( consultarCine(api.colaEnvio, api.colaLogin, login, api.pid, respuesta) == -1 ){
		return false;
	}

	if( respuesta.resultado == RESULTADOCONSULTAERRONEA )
			return false;

	api.estaAutenticado = true;
	return true;
}


bool APIPedirSalas(struct ApiMensajes &api, mensaje &salas)
{
	if(!api.estaAutenticado)
		return false;

	mensaje pedirSalas;
	pedirSalas.mtype = api.pid;
	pedirSalas.salaE.userid = api.pid;
	pedirSalas.tipoMensaje = PEDIR_SALAS;

	if (consultarCine(api.colaEnvio, api.colaRecepcion, pedirSalas, api.pid, salas) == -1)
		return false;

	if(salas.resultado == RESULTADOCONSULTAERRONEA)
		return false;

	return true;		
}


bool APIPedirAsientosSala(struct ApiMensajes &api, int idSala, mensaje &estadoAsientos)
{
	if(!api.estaAutenticado)
		return false;

	mensaje pedirSala;
	pedirSala.mtype = api.pid;
	pedirSala.salaE.userid = api.pid;
	pedirSala.tipoMensaje = ELEGIR_SALA;
	pedirSala.salaE.salaid = idSala;

	if(consultarCine(api.colaEnvio, api.colaRecepcion, pedirSala, api.pid, estadoAsientos) == -1){
		return false;
	}

	if(estadoAsientos.resultado == RESULTADOCONSULTAERRONEA )
		return false;

	//Entra en la sala.
	api.salaActual = idSala;
	return true;
}

//Obtiene el estado de los asientos.
bool APIObtenerActualizacionAsientos(struct ApiMensajes &api, struct sala &estadoAsientos)
{
	tomarSem(api.mutexMemoriaCompartida);

	estadoAsientos = *api.memoriaCompartida;

	liberarSem(api.mutexMemoriaCompartida);

	return true;
}

bool APIElegirAsiento(struct ApiMensajes &api, mensaje &asiento)
{
	if(!api.estaAutenticado)
		return false;

	mensaje respuesta;

	asiento.asientoS.idSala = api.salaActual;
	asiento.asientoS.estado = ASIENTORESERVADO;
	asiento.asientoS.idUsuario = api.pid;
	asiento.mtype = api.pid;
	asiento.tipoMensaje = INTERACCION_ASIENTO;

	if(consultarCine(api.colaEnvio, api.colaRecepcion, asiento, api.pid, respuesta) == -1)
		return false;

	if(respuesta.resultado == RESULTADOERROR )
		return false;

	return true;
}

bool APIFinalizarCompra(struct ApiMensajes &api)
{
	if(!api.estaAutenticado)
		return false;

	mensaje respuesta;
	mensaje comprar;
	comprar.mtype = api.pid;
	comprar.tipoMensaje = FINALIZAR_COMPRA;
	comprar.fCompra.userid = api.pid;

	//Finaliza la compra.

	if(consultarCine(api.colaEnvio, api.colaRecepcion, comprar, api.pid, respuesta) == -1)
		return false;
	
	if( respuesta.resultado == RESULTADOCONSULTAERRONEA )
		return false;

	return true;
}

bool asientoValido(struct ApiMensajes &api, unsigned int x, unsigned int y)
{
	tomarSem(api.mutexMemoriaCompartida);

	struct reserva res = api.memoriaCompartida->estadoAsientos[x][y];

	liberarSem(api.mutexMemoriaCompartida);

	return (res.estado == ESTADOASIENTOLIBRE); 
}

bool APIDeinit(struct ApiMensajes &api)
{
	mensaje respuesta;
	mensaje cerrar;
	cerrar.tipoMensaje = SALIR_SALA;
	cerrar.idUsuario = api.pid;

	if(consultarCine(api.colaEnvio, api.colaRecepcion, cerrar, api.pid, respuesta) == -1)
		return false;

	if( respuesta.resultado == RESULTADOCONSULTAERRONEA )
		return false;

	//Finaliza proceso hijo
	kill(api.pidAsincrono,SIGINT);
	wait(NULL);

	//Destruye los recursos.
	cmpMemDestruir(api.idMemoriaCompartida, api.memoriaCompartida);
	destruirSem(api.mutexMemoriaCompartida);

	//Aca se destruirian los sockets


	return true;
}
