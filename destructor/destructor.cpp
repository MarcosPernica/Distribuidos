#include "ipc/cola.h"
#include "ipc/semaforo.h"
#include "ipc/memoriacompartida.h"
#include "common/common.h"

int main(int argc, char* argv)
{
	destruirCola(COLA_LOGIN_CINE);
	destruirCola(COLA_RECEPCION_CINE);
	destruirCola(COLA_ENVIO_CINE);
	destruirCola(COLA_RECEPCION_ADMIN);
	destruirCola(COLA_ASINC_CLIENTE);

	//crear semaforos

	//crear memoria compartida
}