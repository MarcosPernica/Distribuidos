#include <unistd.h>
#include <sys/wait.h>
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "common.h"
#include "mensajes.h"
#include "clienteAsinc.h"
#include <stdio.h>

sig_atomic_t vivo = 0;

void terminar(int sigint){
	vivo = 1;
}

bool entro(mensaje login){
	return true;
}

void obtenerDatosLogin(mensaje &login)
{
}

int main(int argc, char** argv){
	int registrar = registrarSenal(SIGINT,terminar);

	//Obtiene la cola de identificacion con el cine
	int colaRespuesta = obtenerCola(COLA_LOGIN_CINE);

	//Obtiene las colas de comunicaciones con el cine hijo
	int colaEnvio = obtenerCola(COLA_RECEPCION_CINE);
	int colaRecepcion = obtenerCola(COLA_ENVIO_CINE);



	while (vivo == 0 && !autenticado){
		
	}

	if (fork() == 0){
		correrAsincronico(getpid());
	}

	bool terminado = false;

	while(vivo == 0 && !terminado){
		//hacer request al usuario
		//enviarlo y ver respuesta
		
	}

	return 0;
}

static bool autenticar(int colaRespuesta, int pid)
{
	mensaje login;
	login.mtype = LOGIN_TYPE;
	login.l.id = pid;

	mensaje respuesta;

	//Obtiene los datos por consola
	obtenerDatosLogin(login);

	if( enviarMensaje(colaRespuesta,(void *)&login,sizeof(login)) < 0 )
		return false
	
	if (recibirMensaje(colaRespuesta,pid,(void*)&respuesta,sizeof(respuesta)) < 0 )
		if(respuesta.resultado == RESULTADOCONSULTAERRONEA)
			return false
	
	if (entro(respuesta))
		return true;
}

static mensaje consultarCine(int colaEnvio, int colaRecepcion, mensaje &consulta)
{
	mensaje respuesta;

	enviarMensaje(colaEnvio,(void *)&consulta,sizeof(login));
	recibirMensaje(colaRecepcion,pid,(void*)&respuesta,sizeof(respuesta));

	return respuesta;
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
		for(unsigned int i=0; i < salas.totalSalas; i++)
		{
			printf("Sala %d\n",i);
		}
		fflush(stdin);
		printf("Opcion: ");
		scanf("%d",&opcion);
	}
	while(opcion > salas.totalSalas);
	
	return opcion;
}

static struct asiento mostrarYElegirAsiento(const struct sala &sala)
{
	int entradaIncorrecta = 0;
	unsigned int x = 0, y = 0;
	do
	{
		entradaIncorrecta++;
		system("clear");
		if(entradaIncorrecta > 2)
			printf("Asiento no valido. Intente nuevamente.\n");

		printf("Seleccione el asiento que desea reservar:\n");
		fflush(stdin);
		printf("Asiento (fila,columna): ");
		scanf("%d %d",&x, &y);
	}
	while(x > CANTIDADMAXASIENTOS || y > CANTIDADMAXASIENTOS);

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

		printf("Seleccione como desea proceder:\n");
		fflush(stdin);
		printf("0 - Reintentar operacion desde el principio.");
		printf("1 - Cerrar");
		scanf("%d",&opcion);
	}
	while(opcion > 1);
	
	return opcion;
}

static bool maquinaEstadosCliente(int colaEnvio, int colaRecepcion, int colaRespuesta)
{
	mensaje pedirSalas;
	mensaje pedirSala;
	mensaje reservar;
	mensaje comprar;

	bool eligioCerrar = false;
	int pid = getpid();
	if(!autenticar(colaRespuesta, pid))
	{
		printf("Error al autenticar con servidor.\n");
		exit(0);
	}

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

	while(!eligioCerrar)
	{
		//Pide las salas.
		mensaje salas = consultarCine(colaEnvio, colaRecepcion, pedirSalas);

		if(salas.resultado == RESULTADOCONSULTAERRONEA)
		{
			eligioCerrar = mostrarMensajeError();
			continue;
		}

		//Envia la seleccion del usuario y recibe el estado de la sala.
		pedirSala.salaE.salaid = mostrarYElegirSala(salas.salaP);

		mensaje sala = consultarCine(colaEnvio, colaRecepcion, &pedirSala);

		if(sala.resultado == RESULTADOCONSULTAERRONEA)
		{
			eligioCerrar = mostrarMensajeError();
			continue;
		}

		//Elige los asientos

		elegirAsientos()

			
	}
	
}
