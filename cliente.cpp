#include <unistd.h>
#include "common.h"
#include "mensajes.h"
#include <stdio.h>
#include <stdlib.h>
#include "apiMensajes/apiMensajes.h"
#include <string.h>

void obtenerDatosLogin(login &login)
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

	login.user = user;
}

static unsigned int mostrarYElegirSala(const struct salas &salas)
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

static void imprimirSala(sala sala){
	printf("\t ASIENTOS \t\n");
	printf("  ");
	for (int i = 0; i < CANTIDADMAXASIENTOS; i++ ){
		printf("%d ",i);
	}
	printf("\n");

	for (int i = 0; i < CANTIDADMAXASIENTOS; i++ ){
		printf("%d ",i);
		for (int j = 0; j < CANTIDADMAXASIENTOS; j++){
			reserva seat = sala.estadoAsientos[i][j];
			printf("%c ",seat.estado);
		}
		printf("\n");
	}
}

static bool asientoValido(sala sala, asiento asiento){
	if( sala.estadoAsientos[asiento.x][asiento.y].estado != ESTADOASIENTOLIBRE )
		return false;
	return true;
}

static struct asiento mostrarYElegirAsiento(sala &sala)
{
	int entradaIncorrecta = 0;
	asiento asiento;
	do
	{
		entradaIncorrecta++;
		system("clear");
		imprimirSala(sala);
		if(entradaIncorrecta > 2)
			printf("Asiento no valido. Intente nuevamente.\n");

		printf("Seleccione el asiento que desea reservar:\n");
		fflush(stdin);
		printf("Asiento (fila,columna): ");
		scanf("%d %d",&asiento.x, &asiento.y);
	}
	while((asiento.x > CANTIDADMAXASIENTOS || asiento.y > CANTIDADMAXASIENTOS)
			&& !asientoValido(sala, asiento));
	
	return asiento;
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

static bool mostrarMenuMasReservas(bool huboError)
{
	int entradaIncorrecta = 0;
	unsigned opcion = 0;
	do
	{
		entradaIncorrecta++;
		system("clear");
		if( huboError ){
			printf("Error al reservar el asiento, ya estaba reservado.\n");
		}
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
	int fd = APIInit();
	if( fd == -1 )
		return false;
	login login;

	bool eligioCerrar = false;

	obtenerDatosLogin(login);
	if( ! APILogin(fd,login) )
	{
		printf("Error al autenticar con servidor.\n");
		return false;
	}

	while(!eligioCerrar)
	{
		//Pide las salas.
		salas salas;
		if( ! APIPedirSalas(fd,salas) )
		{
			eligioCerrar = mostrarMensajeError();
			continue;
		}

		//Envia la seleccion del usuario y recibe el estado de la sala.
		sala sala;
		if(!APIPedirAsientosSala(fd, mostrarYElegirSala(salas), sala))
		{
			eligioCerrar = mostrarMensajeError();
			continue;
		}

		bool masReservas = true;
		bool errorReserva;
		asiento asiento;
		do
		{
			asiento = mostrarYElegirAsiento(sala);
			errorReserva = false;
			if( ! APIElegirAsiento(fd, asiento) )
			{
				errorReserva = true;
			}
			APIObtenerActualizacionAsientos(fd,sala);
			//Muestra el menu
			masReservas = mostrarMenuMasReservas(errorReserva);
		}
		while(masReservas);

		//Finaliza la compra
		
		if(!APIFinalizarCompra(fd))
		{
			eligioCerrar = mostrarMensajeError();
			continue;
		}

		eligioCerrar = true;	
	}
	APIDeinit(fd);
	return true;
}

int main(int argc, char** argv){

	//Lanza la interfaz en si.
	maquinaEstadosCliente();

	return 0;
}
