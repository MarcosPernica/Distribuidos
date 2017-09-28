#include "mom.h"
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
#include <string.h>

sig_atomic_t alarma = 0;

static void alarmHandler(int sigint){
	alarma = 1;
}

static int consultarCine(int colaEnvio, int colaRecepcion, mensaje &consulta, int pid, mensaje &respuesta)
{

	if ( enviarMensaje(colaEnvio,(void *)&consulta,sizeof(mensaje)) == -1 ){
		return -1;
	}

	ponerAlarma(TIME_OUT_CONNECTION);
	if( recibirMensaje(colaRecepcion,pid, (void *)&respuesta,sizeof(mensaje)) == -1 ){

		if( ocurrioAlarma(alarma) ){
			printf("Timeout client\n");
			recibirMensajeAsinc(colaEnvio,pid,(void*)&respuesta,sizeof(mensaje));
			respuesta.resultado = RESULTADOERROR;
		} else {
			printf("Error recibiendo mensaje al cine\n");
		}
		return -1;
	}
	cancelarAlarma();

	return 0;
}

bool MOMInit(MOM &mom)
{
	mom.lastFD = 0;
	mom.colaLogin = obtenerCola(COLA_LOGIN_CINE);
	mom.colaEnvio = obtenerCola(COLA_RECEPCION_CINE);
	mom.colaRecepcion = obtenerCola(COLA_ENVIO_CINE);
	if( mom.colaLogin == -1 || mom.colaEnvio == -1 || mom.colaRecepcion == -1)
	{
		return false;
	}

	return true;
}

int MOMInitClient(MOM &mom, int ID)
{
	//TODO faltaria verificar que no se uso ya ese id
	//Obtiene la cola de identificacion con el cine
	MOMClient api;

	//Crea la memoria compartida y el mutex para accederla.
	if ((api.idMemoriaCompartida = cmpMemCrear(sizeof(struct sala), ID)) < 0)
		return -1;

	if ((api.mutexMemoriaCompartida = crearSem(ID, 1)) == -1 )
		return -1;

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
	api.fd = ++mom.lastFD;
	mom.clients[api.fd] = api;
	return api.fd;
}


bool MOMLogin(MOM &mom,int fd, struct login &login)
{
	MOMClient client = mom.clients[fd];
	if( client.fd == -1  ) return false;
	if( !client.estaInicializado || client.estaAutenticado )
		return false;

	mensaje envio;
	login.id = client.pid;
	envio.mtype = LOGIN_TYPE;
	envio.l = login;

	mensaje respuesta;
	if ( consultarCine(mom.colaLogin, mom.colaRecepcion, envio, login.id, respuesta) == -1 ){
		return false;
	}

	if( respuesta.resultado == RESULTADOCONSULTAERRONEA )
			return false;

	client.estaAutenticado = true;
	mom.clients[fd] = client;
	return true;
}


bool MOMPedirSalas(MOM &mom, int fd, struct salas &salas)
{
	MOMClient client = mom.clients[fd];
	if( client.fd == -1 ) return false;
	if(!client.estaAutenticado)
		return false;

	mensaje pedirSalas;
	pedirSalas.mtype = client.pid;
	pedirSalas.tipoMensaje = PEDIR_SALAS;

	mensaje respuesta;
	if (consultarCine(mom.colaEnvio, mom.colaRecepcion, pedirSalas, client.pid, respuesta) == -1)
		return false;

	if(respuesta.resultado == RESULTADOCONSULTAERRONEA)
		return false;

	salas = respuesta.salaP.salas;
	return true;
}


bool MOMPedirAsientosSala(MOM &mom,int fd, int idSala, struct sala &estadoAsientos)
{
	MOMClient client = mom.clients[fd];
	if( client.fd == -1  ) return false;

	if(!client.estaAutenticado)
		return false;

	mensaje pedirSala;
	pedirSala.mtype = client.pid;
	pedirSala.salaE.userid = client.pid;
	pedirSala.tipoMensaje = ELEGIR_SALA;
	pedirSala.salaE.salaid = idSala;
	mensaje respuesta;
	if( consultarCine(mom.colaEnvio, mom.colaRecepcion, pedirSala, client.pid, respuesta) == -1){
		return false;
	}

	if(respuesta.resultado == RESULTADOCONSULTAERRONEA )
		return false;

	//Entra en la sala.
	client.salaActual = idSala;
	mom.clients[fd] = client;

	copiarAsientos(estadoAsientos.estadoAsientos, respuesta.informacionSala.estadoAsientos);
	estadoAsientos.id = respuesta.informacionSala.id;
	return true;
}

//Obtiene el estado de los asientos.
bool MOMObtenerActualizacionAsientos(MOM &mom, int fd, struct sala &estadoAsientos)
{
	MOMClient client = mom.clients[fd];
	if( client.fd == -1  ) return false;
	if( !client.estaAutenticado ) return false;

	tomarSem(client.mutexMemoriaCompartida);

	estadoAsientos = *client.memoriaCompartida;

	liberarSem(client.mutexMemoriaCompartida);

	return true;
}

bool MOMElegirAsiento(MOM &mom, int fd, struct asiento &asiento)
{
	MOMClient client = mom.clients[fd];
	if( client.fd == -1 ) return false;
	if( !client.estaAutenticado ) return false;

	mensaje respuesta;
	mensaje asientoE;
	asientoE.asientoS.asiento = asiento;
	asientoE.asientoS.idSala = client.salaActual;
	asientoE.asientoS.estado = ASIENTORESERVADO;
	asientoE.asientoS.idUsuario = client.pid;
	asientoE.mtype = client.pid;
	asientoE.tipoMensaje = INTERACCION_ASIENTO;

	if(consultarCine(mom.colaEnvio, mom.colaRecepcion, asientoE, client.pid, respuesta) == -1)
		return false;

	if(respuesta.resultado == RESULTADOERROR )
		return false;

	return true;
}

bool MOMFinalizarCompra(MOM &mom, int fd)
{
	MOMClient client = mom.clients[fd];
	if( client.fd == -1  ) return false;
	if( !client.estaAutenticado ) return false;

	mensaje respuesta;
	mensaje comprar;
	comprar.mtype = client.pid;
	comprar.tipoMensaje = FINALIZAR_COMPRA;
	comprar.fCompra.userid = client.pid;

	//Finaliza la compra.

	if( consultarCine(mom.colaEnvio, mom.colaRecepcion, comprar, client.pid, respuesta) == -1)
		return false;

	if( respuesta.resultado == RESULTADOCONSULTAERRONEA )
		return false;

	return true;
}

bool asientoValido(MOM& mom, int fd, struct asiento &asiento)
{
	MOMClient client = mom.clients[fd];
	if( client.fd == -1 ) return false;
	if( !client.estaAutenticado ) return false;
	tomarSem(client.mutexMemoriaCompartida);

	struct reserva res = client.memoriaCompartida->estadoAsientos[asiento.x][asiento.y];

	liberarSem(client.mutexMemoriaCompartida);

	return (res.estado == ESTADOASIENTOLIBRE);
}

bool MOMDeinit(MOM &mom, int fd)
{
	MOMClient client = mom.clients[fd];
	if( client.fd == -1 ) return true;

	mensaje respuesta;
	mensaje cerrar;
	cerrar.tipoMensaje = SALIR_SALA;
	cerrar.idUsuario = client.pid;

	//envia salio de sala
	consultarCine(mom.colaEnvio, mom.colaRecepcion, cerrar, client.pid, respuesta);

	//Finaliza proceso hijo
	kill(client.pidAsincrono,SIGINT);
	wait(NULL);

	//Destruye los recursos.
	cmpMemDestruir(client.idMemoriaCompartida, client.memoriaCompartida);
	destruirSem(client.mutexMemoriaCompartida);

	mom.clients.erase(fd);
	return true;
}


