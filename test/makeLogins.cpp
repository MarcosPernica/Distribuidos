#include "../ipc/cola.h"
#include "../mensajes.h"
#include "../common.h"

int main(int argc, char** argv)
{
	int cola = obtenerCola(COLA_LOGIN_CINE);

	login log;
	log.id = 1;
	user us;
	us.nombreCliente = "hola";
	us.password = "pepe";
	log.user = us;
	mensaje login;
	login.mtype = LOGIN;
	login.tipoMensaje = LOGIN;

	login.l = log;
	enviarMensaje(cola,(void*)&login, sizeof(mensaje));

	log.id = 3;
	login.l = log;
	enviarMensaje(cola,(void*)&login, sizeof(mensaje));
}
