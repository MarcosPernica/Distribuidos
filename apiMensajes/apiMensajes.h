#ifndef APIMENSAJES
#define APIMENSAJES

#include "../common.h"
#include "../mensajes.h" 
#include "../clienteAsinc.h"

struct ApiMensajes
{
	int colaRecepcion;
	int colaEnvio;
	int colaLogin;
	int salaActual;
	bool estaInicializado;
	bool estaAutenticado;
	int pid;
	
	//Para el manejo de actualizaciones.
	int pidAsincrono;
	int mutexMemoriaCompartida;
	int idMemoriaCompartida;
	struct sala *memoriaCompartida;

	ApiMensajes() : estaInicializado(false), estaAutenticado(false), pid(0), salaActual(-1), pidAsincrono(0), mutexMemoriaCompartida(0), memoriaCompartida(0){}
};

//Inicializa todo lo necesario para poder comunicarse utilizando la API.
bool APIInit(struct ApiMensajes &api, int ID);

//Se logea al sistema.
bool APILogin(struct ApiMensajes &api, mensaje &login);

//Pide las salas.
bool APIPedirSalas(struct ApiMensajes &api, mensaje &salas);

//Pide informacion de los asientos en una sala y entra en la misma.
bool APIPedirAsientosSala(struct ApiMensajes &api, int idSala, mensaje &estadoAsientos);

//Obtiene el estado de los asientos.
bool APIObtenerActualizacionAsientos(struct ApiMensajes &api, struct sala &estadoAsientos);

//Elige un asiento.
bool APIElegirAsiento(struct ApiMensajes &api, mensaje &asiento);

//Valida el asiento X, Y dado.
bool asientoValido(struct ApiMensajes &api, unsigned int x, unsigned int y);

//Finaliza la compra y sale de la sala.
bool APIFinalizarCompra(struct ApiMensajes &api);

//Destruye los recursos.
bool APIDeinit(struct ApiMensajes &api);

#endif

