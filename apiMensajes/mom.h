#ifndef _CINE_MOM_
#define _CINE_MOM_
#include <map>
#include "../entidades.h"

struct MOMClient
{
	int fd;
	int salaActual;
	bool estaInicializado;
	bool estaAutenticado;
	int pid;

	//Para el manejo de actualizaciones.
	int pidAsincrono;
	int mutexMemoriaCompartida;
	int idMemoriaCompartida;
	struct sala *memoriaCompartida;

	MOMClient() : estaInicializado(false), estaAutenticado(false),
			pid(0), salaActual(-1), pidAsincrono(0), mutexMemoriaCompartida(0),
			memoriaCompartida(0),fd(-1), idMemoriaCompartida(1){}
};

struct MOM {
	std::map<int,MOMClient> clients;
	int colaRecepcion;
	int colaEnvio;
	int colaLogin;
	int lastFD;
};

//Inicializa el mom
bool MOMInit(MOM &mom);

//Inicializa un cliente nuevo en el mom
int MOMInitClient(MOM &mom ,int ID, std::string server, int port);

//Se logea al sistema.
bool MOMLogin(MOM &mom, int fd, struct login &login);

//Pide las salas.
bool MOMPedirSalas(MOM &mom, int fd, struct salas &salas);

//Pide informacion de los asientos en una sala y entra en la misma.
bool MOMPedirAsientosSala(MOM &mom, int fd, int idSala, struct sala &estadoAsientos);

//Obtiene el estado de los asientos.
bool MOMObtenerActualizacionAsientos(MOM &mom, int fd, struct sala &estadoAsientos);

//Elige un asiento.
bool MOMElegirAsiento(MOM &mom, int fd, struct asiento &asiento);

//Valida el asiento X, Y dado.
bool MOMasientoValido(MOM &mom, int fd, struct asiento &asiento);

//Finaliza la compra y sale de la sala.
bool MOMFinalizarCompra(MOM &mom, int fd);

//Destruye los recursos.
bool MOMDeinit(MOM &mom, int fd);

int runMOM();

#endif
