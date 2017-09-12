#include "clienteAsinc.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "common.h"
#include "mensajes.h"
#include "ipc/memoriacompartida.h"
#include "ipc/semaforo.h"


sig_atomic_t asinc_vivo = 1;

void terminarAsinc(int sigint){
	asinc_vivo = 0;
}

void correrAsincronico(int pid){
	int cola = obtenerCola(COLA_ASINC_CLIENTE);
	int registro = registrarSenal(SIGINT,terminarAsinc);
	mensaje mensaje;


	//Obtiene la memoria compartida y el mutex de la memoria.
	int idMemoria = cmpMemObtener(sizeof(struct sala), IDMEMORIACOMPARTIDA);
	int semaforo = obtenerSem(IDMUTEX);

	struct sala *informacionSala = (struct sala*) cmpMemObtenerMemoria(idMemoria);
	
	while (asinc_vivo){
		recibirMensaje(cola,pid,(void *)&mensaje,sizeof(mensaje));
		
		//Bloquea el acceso al mapa en memoria.

		tomarSem(semaforo);

		//Lo actualiza y libera.

		*informacionSala = mensaje.informacionSala;

		liberarSem(semaforo);
	}

	cmpMemDestruir(idMemoria, informacionSala);
}
