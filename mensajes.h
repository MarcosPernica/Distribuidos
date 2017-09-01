#ifndef MENSAJES_CINE
#define MENSAJES_CINE
#include "entidades.h"

#define LOGIN_TYPE 1

#define LOGIN 1
#define LOGIN_RESPONSE 2
#define PEDIR_SALAS 3
#define ELEGIR_SALA 4
#define ELEGIR_ASIENTO 5
#define FINALIZAR_COMPRA 6
#define ENTRO_A_SALA 7
#define ASIENTO_RESERVADO 8
#define ASIENTO_COMPRADO 9
#define SALIO_DE_SALA 10

//mensajes cliente-cine
struct login {
	int id;
	struct user user;
};

struct loginResponse{
	int success;
};

struct pedirSalas{
	struct salas salas;
};

struct elegirSala{
	int entrar;
	struct sala sala;
};

struct asientoSeleccionado{
	int selecciono;
	struct asiento asiento;
};

struct finalizarCompra {
	int compro;
};

//mensajes cine-admin
struct interaccionSala {
	int pid;
	int entro;
	struct sala sala;
};

struct asientoPedido {
	int pid;
	int accion; //reservo,compro,libero
	struct asiento asiento;
};

struct mensaje{
	long mtype;
	int tipoMensaje;
	union {
		struct login l;
		struct loginResponse lresponse;
		struct pedirSalas salaP;
		struct elegirSala salaE;
		struct asientoSeleccionado asientoS;
		struct finalizarCompra fCompra;

		struct interaccionSala sala;
		struct asientoPedido aPedido;

	};
};

#endif
