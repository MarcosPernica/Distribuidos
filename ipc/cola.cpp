#include "cola.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int crearCola(int id){
	key_t clave;
	clave = ftok("/bin/bash", id);
	return (msgget(clave,  IPC_CREAT | IPC_EXCL | 0660));
}

int obtenerCola(int id){
	key_t clave;
	clave = ftok("/bin/bash", id);
	return (msgget(clave,  IPC_CREAT | 0660));
}

int destruirCola(int id){
	int cola = obtenerCola(id);
	return (msgctl(cola, IPC_RMID, NULL));
}

int enviarMensaje(int colaId,void* data, int length){
	return msgsnd(colaId,data,length-sizeof(long),0);
}

int recibirMensaje(int colaId, void* buffer, int length){
	return msgrcv(colaId,buffer,length-sizeof(long),0,0);
}

int recibirMensaje(int colaId, long type ,void* buffer, int length){
	return msgrcv(colaId,buffer,length-sizeof(long),type,0);
}

int recibirMensajeAsinc(int colaId, long type ,void* buffer, int length){
	return msgrcv(colaId,buffer,length-sizeof(long),type,IPC_NOWAIT);
}

