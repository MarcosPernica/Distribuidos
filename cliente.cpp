#include <unistd.h>
#include <sys/wait.h>
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "ipc/memoriacompartida.h"
#include "ipc/semaforo.h"
#include "common.h"
#include "mensajes.h"
#include "clienteAsinc.h"
#include <stdio.h>
#include <stdlib.h>
#include "timeout.h"


sig_atomic_t vivo = 0;

void terminar(int sigint){
	vivo = 1;
}

sig_atomic_t alarma = 0;

void alarmHandler(int sigint){
	alarma = 1;
}

void obtenerDatosLogin(mensaje &login)
{
	char stdinRead[1024];
	user user;
	int lenRead;
	int error = 0;
	do
	{
		system("clear");
		if( error > 0 ){
			printf("Por favor ingrese un dato de menos de %d caracteres \n", LONGITUDMAXNOMBRE);
		}
		error = 1;
		printf("Ingrese su nombre de usuario: \n");
		fflush(stdin);
		scanf("%1023s",stdinRead);
		lenRead = strlen(stdinRead);
		if( lenRead < LONGITUDMAXNOMBRE ){
			strcpy(user.nombreCliente,stdinRead);
			break;
		}
	} while(true);

	error = 0;

	do
	{
		if( error > 0 ){
			printf("Por favor ingrese un dato de menos de %d caracteres \n", LONGITUDMAXNOMBRE);
		}
		error = 1;
		printf("Ingrese su contrasena: \n");
		fflush(stdin);
		scanf("%1023s",stdinRead);
		lenRead = strlen(stdinRead);
		if( lenRead < LONGITUDMAXNOMBRE ){
			strcpy(user.password,stdinRead);
			break;
		}
	} while(true);

	login.l.user = user;
}

void crearRecursos()
{
	//Crea la memoria compartida y el mutex para accederla.
	if ( cmpMemCrear(sizeof(struct sala), getpid()) < 0){
		printf("Ya existia la memoria compartido\n");
		exit(1);
	}
	if ( crearSem(getpid(), 1) == -1 ){
		printf("Ya existia el semaforo de la memoria compartida\n");
		exit(1);
	}
}

void destruirRecursos(void *memoria, int idMemoria, int idSemaforo)
{
	//Destruye la memoria compartida y el mutex para accederla.
	cmpMemDestruir(idMemoria, memoria);
	destruirSem(idSemaforo);
}

static int consultarCine(int colaEnvio, int colaRecepcion, mensaje &consulta, int pid,mensaje &respuesta)
{

	if ( enviarMensaje(colaEnvio,(void *)&consulta,sizeof(mensaje)) == -1 ){
		printf("Error enviando mensaje al cine\n");
		return -1;
	}

	ponerAlarma(TIME_OUT_CONNECTION);
	while ( recibirMensaje(colaRecepcion,pid,(void*)&respuesta,sizeof(mensaje)) == -1 ){
		if( ocurrioAlarma(alarma) ){
			printf("No se puede comunicar con el cine\n");
			//remuevo mensaje puesto
			recibirMensajeAsinc(colaEnvio,pid,(void*)&respuesta,sizeof(mensaje));
		}
		return -1;
	}
	cancelarAlarma();
	return 0;

}

static unsigned int mostrarYElegirSala(const struct pedirSalas &salas)
{
	int entradaIncorrecta = 0;
	unsigned int opcion = 0;
	do
	{
		entradaIncorrecta++;
		system("clear");
		if(entradaIncorrecta > 2)
			printf("Opcion incorrecta. Intente nuevamente.\n");

		printf("Seleccione la sala en la que desea reservar:\n");
		for(unsigned int i=0; i < salas.salas.totalSalas; i++)
		{
			printf("Sala %d\n",i);
		}
		fflush(stdin);
		printf("Opcion: ");
		scanf("%d",&opcion);
	}
	while(opcion > salas.salas.totalSalas);
	
	return opcion;
}

static bool asientoValido(struct sala *informacionSala, int idSemaforo, unsigned int x, unsigned int y)
{
	tomarSem(idSemaforo);

	struct reserva res = informacionSala->estadoAsientos[x][y];

	liberarSem(idSemaforo);

	return (res.estado == ESTADOASIENTOLIBRE); 
}

static void imprimirSala(struct sala *informacionSala, int idSemaforo){
	tomarSem(idSemaforo);
	printf("\t ASIENTOS \t\n");
	printf("  ");
	for (int i = 0; i < CANTIDADMAXASIENTOS; i++ ){
		printf("%d ",i);
	}
	printf("\n");

	for (int i = 0; i < CANTIDADMAXASIENTOS; i++ ){
		printf("%d ",i);
		for (int j = 0; j < CANTIDADMAXASIENTOS; j++){
			reserva seat = informacionSala->estadoAsientos[i][j];
			printf("%c ",seat.estado);
		}
		printf("\n");
	}
	fflush(stdin);
	liberarSem(idSemaforo);
}

static struct asiento mostrarYElegirAsiento(struct sala *informacionSala, int idSemaforo)
{
	int entradaIncorrecta = 0;
	unsigned int x = 0, y = 0;
	do
	{
		entradaIncorrecta++;
		system("clear");
		imprimirSala(informacionSala,idSemaforo);
		if(entradaIncorrecta > 2)
			printf("Asiento no valido. Intente nuevamente.\n");

		printf("Seleccione el asiento que desea reservar:\n");
		fflush(stdin);
		printf("Asiento (fila,columna): ");
		scanf("%d %d",&x, &y);
	}
	while((x > CANTIDADMAXASIENTOS || y > CANTIDADMAXASIENTOS) && !asientoValido(informacionSala, idSemaforo, x, y));

	struct asiento seleccion{x,y};
	
	return seleccion;
}

static bool mostrarMensajeError()
{
	int entradaIncorrecta = 0;
	unsigned opcion = 0;
	do
	{
		entradaIncorrecta++;
		system("clear");
		if(entradaIncorrecta > 2)
			printf("Opcion incorrecta. Intente nuevamente.\n");

		printf("Hubo un error, seleccione como desea proceder:\n");
		fflush(stdin);
		printf("0 - Reintentar operacion desde el principio.\n");
		printf("1 - Cerrar.\n");
		scanf("%d",&opcion);
	}
	while(opcion > 1);
	
	return opcion;
}

static bool mostrarMenuMasReservas()
{
	int entradaIncorrecta = 0;
	unsigned opcion = 0;
	do
	{
		entradaIncorrecta++;
		system("clear");
		if(entradaIncorrecta > 2)
			printf("Opcion incorrecta. Intente nuevamente.\n");

		printf("Seleccione como desea proceder:\n");
		fflush(stdin);
		printf("0 - Hacer otra reserva.\n");
		printf("1 - Comprar\n");
		scanf("%d",&opcion);
	}
	while(opcion > 1);
	
	return !opcion;
}

static bool autenticar(int colaEnvio,int colaRespuesta, int pid)
{
	mensaje login;
	login.mtype = LOGIN_TYPE;
	login.l.id = pid;
	mensaje respuesta;

	obtenerDatosLogin(login);
	if ( consultarCine(colaEnvio,colaRespuesta,login,pid,respuesta) == -1 ){
		return false;
	}

	if( respuesta.resultado == RESULTADOCONSULTAERRONEA )
			return false;

	return true;
}

static bool maquinaEstadosCliente(int colaEnvio, int colaRecepcion, int colaLogin, struct sala *informacionSala, int idSemaforo)
{
	mensaje pedirSalas;
	mensaje pedirSala;
	mensaje reservar;
	mensaje comprar;
	mensaje respuesta;

	bool eligioCerrar = false;
	int pid = getpid();
	if( !autenticar(colaLogin,colaRecepcion, pid) )
	{
		printf("Error al autenticar con servidor.\n");
		return false;
	}

	//Llena en las estructuras los campos comunes.
	pedirSalas.mtype = pid;
	pedirSalas.salaE.userid = pid;
	pedirSalas.tipoMensaje = PEDIR_SALAS;


	pedirSala.mtype = pid;
	pedirSala.salaE.userid = pid;
	pedirSala.tipoMensaje = ELEGIR_SALA;

	reservar.mtype = pid;
	reservar.tipoMensaje = INTERACCION_ASIENTO;

	comprar.mtype = pid;
	comprar.tipoMensaje = FINALIZAR_COMPRA;
	comprar.fCompra.userid = pid;

	while(!eligioCerrar)
	{
		//Pide las salas.
		mensaje salas;
		if ( consultarCine(colaEnvio, colaRecepcion, pedirSalas, pid,salas) == -1){
			return false;
		}

		if(salas.resultado == RESULTADOCONSULTAERRONEA)
		{
			eligioCerrar = mostrarMensajeError();
			continue;
		}

		//Envia la seleccion del usuario y recibe el estado de la sala.
		pedirSala.salaE.salaid = mostrarYElegirSala(salas.salaP);

		mensaje sala;
		if( consultarCine(colaEnvio, colaRecepcion, pedirSala, pid,sala) == -1){
			return false;
		}

		if( sala.resultado == RESULTADOCONSULTAERRONEA )
		{
			eligioCerrar = mostrarMensajeError();
			continue;
		}

		//Guarda el estado inicial de la sala.
		printf("semaphore section\n");
		tomarSem(idSemaforo);
		*informacionSala = sala.informacionSala;
		liberarSem(idSemaforo);
		printf("semaphore section out\n");

		bool masReservas = true;

		//Elige los asientos
		reservar.asientoS.idSala = pedirSala.salaE.salaid;
		reservar.asientoS.estado = ASIENTORESERVADO;
		reservar.asientoS.idUsuario = pid;

		do
		{
			reservar.asientoS.asiento = mostrarYElegirAsiento(informacionSala, idSemaforo);
		
			if( consultarCine(colaEnvio, colaRecepcion, reservar, pid,respuesta) == -1){
				return false;
			}

			if( respuesta.resultado == RESULTADOERROR )
				printf("Error al reservar el asiento, ya estaba reservado.");
			
			//Muestra el menu
			masReservas = mostrarMenuMasReservas();
		}
		while(masReservas);

		//Finaliza la compra
		
		if( consultarCine(colaEnvio, colaRecepcion, comprar, pid, respuesta) == -1){
			break;
		}
		
		if( respuesta.resultado == RESULTADOCONSULTAERRONEA )
		{
			eligioCerrar = mostrarMensajeError();
			continue;
		}

		eligioCerrar = true;	
	}
	mensaje cerrar;
	cerrar.tipoMensaje = SALIR_SALA;
	cerrar.idUsuario = pid;
	consultarCine(colaEnvio,colaRecepcion,cerrar,pid,respuesta);
	return true;
}

int main(int argc, char** argv){
	if( registrarSenal(SIGINT,terminar) == -1){
		printf("Cliente: error al registrar senal");
		exit(1);
	}

	if( registrarSenal(SIGALRM,alarmHandler) == -1 ){
		printf("Cliente: error al registrar senal timeout");
		exit(1);
	}
	//Obtiene la cola de identificacion con el cine
	int colaLogin = obtenerCola(COLA_LOGIN_CINE);

	//Obtiene las colas de comunicaciones con el cine hijo
	int colaEnvio = obtenerCola(COLA_RECEPCION_CINE);
	int colaRecepcion = obtenerCola(COLA_ENVIO_CINE);

	crearRecursos();
	int pid = getpid();
	int pidAsincronico = fork();
	if ( pidAsincronico == 0){
		correrAsincronico(pid);
		exit(0);
	}

	//Obtiene la memoria compartida y el mutex de la memoria.
	int idMemoria = cmpMemObtener(sizeof(struct sala), getpid());
	int idSemaforo = obtenerSem(pid);

	struct sala *informacionSala = (struct sala*) cmpMemObtenerMemoria(idMemoria);
	
	//Lanza la interfaz en si.
	maquinaEstadosCliente(colaEnvio, colaRecepcion, colaLogin, informacionSala, idSemaforo);

	//finaliza proceso hijo
	kill(pidAsincronico,SIGINT);
	wait(NULL);

	//Destruye los recursos
	destruirRecursos(informacionSala, idMemoria, idSemaforo);
	return 0;
}
