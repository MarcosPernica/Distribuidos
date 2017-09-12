#ifndef MENSAJES_CINE
#define MENSAJES_CINE
#include "entidades.h"

#define LOGIN_TYPE 1

#define LOGIN 1
#define LOGIN_RESPONSE 2

#define PEDIR_SALAS 3
#define ELEGIR_SALA 4
#define INTERACCION_ASIENTO 5
#define FINALIZAR_COMPRA 6

#define ASIENTORESERVADO 7
#define ASIENTODESRESERVADO 8

#define RESULTADOOK 9
#define RESULTADOCONSULTAERRONEA 10
#define RESULTADOERROR 11

#define AVISOASINCRONO 12

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
	int userid;
	int salaid;
};

struct finalizarCompra {
	int userid;
};

struct mensaje{
	long mtype;
	int tipoMensaje;
	char resultado;
	union {
		struct login l;
		struct loginResponse lresponse;
		struct pedirSalas salaP;
		struct elegirSala salaE;
		struct sala informacionSala;
		struct reserva asientoS;
		struct finalizarCompra fCompra;
	};
};

#endif
