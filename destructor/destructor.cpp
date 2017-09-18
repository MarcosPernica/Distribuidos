#include "../ipc/cola.h"
#include "../common.h"

int main(int argc, char** argv)
{
	destruirCola(COLA_LOGIN_CINE);
	destruirCola(COLA_RECEPCION_CINE);
	destruirCola(COLA_ENVIO_CINE);
	destruirCola(COLA_ASINC_CLIENTE);
	destruirCola(COLA_RECEPCION_ADMIN);
	destruirCola(COLA_ENVIO_ADMIN);
}


