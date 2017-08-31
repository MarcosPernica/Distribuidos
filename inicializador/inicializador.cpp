#include "ipc/cola.h"
#include "ipc/semaforo.h"
#include "ipc/memoriacompartida.h"
#include "common/common.h"

int main(int argc, char* argv)
{
	crearCola(COLA_LOGIN_CINE);
	crearCola(COLA_RECEPCION_CINE);
	crearCola(COLA_ENVIO_CINE);
	crearCola(COLA_RECEPCION_ADMIN);
	crearCola(COLA_ASINC_CLIENTE);

	//crear semaforos

	//crear memoria compartida
}