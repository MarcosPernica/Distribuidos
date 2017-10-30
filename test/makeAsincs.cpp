
#include "../ipc/cola.h"
#include "../mensajes.h"
#include "../common.h"

int main(int argc, char** argv)
{
	int cola = obtenerCola(COLA_ASINC_CLIENTE);

	struct sala sal;
	reserva res;
	res.estado = ESTADOASIENTOLIBRE;
	res.idSala = 4;
	res.idUsuario = 0;
	asiento as;

	for(unsigned int i = 0; i< CANTIDADMAXASIENTOS; i++){
			for(unsigned int a = 0; a < CANTIDADMAXASIENTOS; a++){
				as.x = 0;
				as.y = 1;
				res.asiento = as;
				sal.estadoAsientos[i][a] = res ;
			}
	}

	mensaje aviso;
	aviso.mtype = 1;
	aviso.tipoMensaje = AVISOASINCRONO;
	aviso.informacionSala = sal;
	enviarMensaje(cola,(void*)&aviso, sizeof(mensaje));

	aviso.mtype = 3;
	enviarMensaje(cola,(void*)&aviso, sizeof(mensaje));
}
