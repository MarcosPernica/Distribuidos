#ifndef APIMENSAJES
#define APIMENSAJES

#include "../mensajes.h" 

//Inicializa todo lo necesario para poder comunicarse utilizando la API.
int APIInit();

//Se logea al sistema.
bool APILogin(int fd, struct login &login);

//Pide las salas.
bool APIPedirSalas(int fd, struct salas &salas);

//Pide informacion de los asientos en una sala y entra en la misma.
bool APIPedirAsientosSala(int fd, int idSala, struct sala &estadoAsientos);

//Obtiene el estado de los asientos.
bool APIObtenerActualizacionAsientos(int fd, struct sala &estadoAsientos);

//Elige un asiento.
bool APIElegirAsiento(int fd, struct asiento &asiento);

//Finaliza la compra y sale de la sala.
bool APIFinalizarCompra(int fd);

//Destruye los recursos.
bool APIDeinit(int fd);

#endif

