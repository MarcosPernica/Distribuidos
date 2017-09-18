#ifndef COLA_MENSAJE
#define COLA_MENSAJE

int crearCola(int id);

int obtenerCola(int id);

int destruirCola(int id);

int enviarMensaje(int colaId,void* data, int length);

int recibirMensaje(int colaId, void* buffer, int length);

int recibirMensaje(int colaId, long type ,void* buffer, int length);

int recibirMensajeAsinc(int colaId, long type ,void* buffer, int length);

#endif
