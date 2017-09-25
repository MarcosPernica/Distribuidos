#include <unistd.h>
#include <sys/wait.h>
#include "ipc/senal.h"
#include "common.h"
#include "mensajes.h"
#include <stdio.h>
#include <stdlib.h>
#include "apiMensajes/apiMensajes.h"

sig_atomic_t vivo = 0;

static void terminar(int sigint){
	vivo = 1;
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

static void imprimirSala(struct ApiMensajes api){
	struct sala informacionSala;
	APIObtenerActualizacionAsientos(api, informacionSala);

	printf("\t ASIENTOS \t\n");
	printf("  ");
	for (int i = 0; i < CANTIDADMAXASIENTOS; i++ ){
		printf("%d ",i);
	}
	printf("\n");

	for (int i = 0; i < CANTIDADMAXASIENTOS; i++ ){
		printf("%d ",i);
		for (int j = 0; j < CANTIDADMAXASIENTOS; j++){
			reserva seat = informacionSala.estadoAsientos[i][j];
			printf("%c ",seat.estado);
		}
		printf("\n");
	}
}

static struct asiento mostrarYElegirAsiento(struct ApiMensajes &api)
{
	int entradaIncorrecta = 0;
	unsigned int x = 0, y = 0;
	do
	{
		entradaIncorrecta++;
		system("clear");
		imprimirSala(api);
		if(entradaIncorrecta > 2)
			printf("Asiento no valido. Intente nuevamente.\n");

		printf("Seleccione el asiento que desea reservar:\n");
		fflush(stdin);
		printf("Asiento (fila,columna): ");
		scanf("%d %d",&x, &y);
	}
	while((x > CANTIDADMAXASIENTOS || y > CANTIDADMAXASIENTOS) && !asientoValido(api, x, y));

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

static bool maquinaEstadosCliente()
{
	struct ApiMensajes api;
	if(!APIInit(api,getpid()))
		return false;

	mensaje reservar;	
	mensaje respuesta;
	mensaje login;

	bool eligioCerrar = false;
	int pid = getpid();

	obtenerDatosLogin(login);

	if(!APILogin(api,login))
	{
		printf("Error al autenticar con servidor.\n");
		return false;
	}


	while(!eligioCerrar)
	{
		//Pide las salas.
		mensaje salas;
		if(!APIPedirSalas(api,salas))
		{
			eligioCerrar = mostrarMensajeError();
			continue;
		}

		//Envia la seleccion del usuario y recibe el estado de la sala.
		mensaje sala;

		if(!APIPedirAsientosSala(api, mostrarYElegirSala(salas.salaP), sala))
		{
			eligioCerrar = mostrarMensajeError();
			continue;
		}

		bool masReservas = true;

		do
		{
			reservar.asientoS.asiento = mostrarYElegirAsiento(api);
		
			if(!APIElegirAsiento(api, reservar))
				printf("Error al reservar el asiento, ya estaba reservado.");
			
			//Muestra el menu
			masReservas = mostrarMenuMasReservas();
		}
		while(masReservas);

		//Finaliza la compra
		
		if(!APIFinalizarCompra(api))
		{
			eligioCerrar = mostrarMensajeError();
			continue;
		}

		eligioCerrar = true;	
	}
	APIDeinit(api);
	return true;
}

int main(int argc, char** argv){
	if( registrarSenal(SIGINT,terminar) == -1){
		printf("Cliente: error al registrar senal");
		exit(1);
	}

	//Lanza la interfaz en si.
	maquinaEstadosCliente();

	return 0;
}
