#include "clienteAsinc.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "common.h"
#include "mensajes.h"

sig_atomic_t asinc_vivo = 0;

void terminarAsinc(int sigint){
	asinc_vivo = 1;
}

void correrAsincronico(int pid){
	int cola = obtenerCola(COLA_ASINC_CLIENTE);
	int registro = registrarSenal(SIGINT,terminarAsinc);
	mensaje mensaje;
	while (asinc_vivo){
		recibirMensaje(cola,pid,(void *)&mensaje,sizeof(mensaje));
		//cambiar lo que tenga q cambiar y listo
	}
}
