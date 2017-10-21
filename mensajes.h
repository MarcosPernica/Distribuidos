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
#define TIMEOUT 13
#define SALIR_SALA 14

#define ASIENTORESERVADO 7
#define ASIENTODESRESERVADO 8

#define RESULTADOOK 9
#define RESULTADOCONSULTAERRONEA 10
#define RESULTADOERROR -1

#define AVISOASINCRONO 12

#define INITMOM 15
#define DESTROY_MOM 16

#define ACTUALIZAR_ASIENTOS 17


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

//mensajes client-mom
struct mominit {
	int pid;
	int fd;
};



struct mensaje{
	long mtype;
	int tipoMensaje;
	char resultado;
	int fd;
	union {
		struct login l; //Corresponde a LOGIN.
		struct loginResponse lresponse; //Corresponde a LOGIN_RESPONSE.
		struct pedirSalas salaP; //Corresponde a PEDIR_SALAS.
		struct elegirSala salaE; //Corresponde al mensaje de ida de ELEGIR_SALA (Es cuando se piden todos los asientos)
		struct sala informacionSala; //Corresponde al mensaje de vuelta de ELEGIR_SALA (Es cuando se devuelven todos los asientos). Tambien a AVISOASINCRONO 
		struct reserva asientoS; //Corresponde a INTERACCION_ASIENTOS.
		struct finalizarCompra fCompra; //Corresponde a FINALIZAR_COMPRA.
		struct mominit initmom;
		int idUsuario;
	};
};

#endif
