#include "ipc/cola.h"
#include "ipc/semaforo.h"
#include "ipc/memoriacompartida.h"
#include "../common.h"

int main(int argc, char* argv)
{
	crearCola(COLA_LOGIN_CINE);
	crearCola(COLA_RECEPCION_CINE);
	crearCola(COLA_ENVIO_CINE);
	crearCola(COLA_ASINC_CLIENTE);
	crearCola(COLA_RECEPCION_ADMIN);
	crearCola(COLA_ENVIO_ADMIN);

	//crear semaforos

	//crear memoria compartida
}
