#ifndef MENSAJES_CINE
#define MENSAJES_CINE
#include "entidades/entidades.h"

#define LOGIN_TYPE 1

#define LOGIN 1
#define LOGIN_RESPONSE 2
#define PEDIR_SALAS 3
#define ELEGIR_SALA 4
#define ELEGIR_ASIENTO 5
#define FINALIZAR_COMPRA 3
#define ENTRO_A_SALA 7
#define ASIENTO_RESERVADO 8
#define ASIENTO_COMPRADO 9
#define SALIO_DE_SALA 10

//mensajes cliente-cine
struct {
	int id;
	user user;
} typedef login;

struct {
	int success;
} typedef loginResponse;
struct {
	salas salas;
} typedef pedirSalas;

struct {
	int entrar;
	sala sala;
} typedef elegirSala;

struct {
	int selecciono;
	asiento asiento;
}typedef asientoSeleccionado;

struct {
	int compro;
} typedef finalizarCompra;

//mensajes cine-admin
struct {
	int pid;
	int entro;
} typedef interaccionSala;

struct {
	int pid;
	int accion; //reservo,compro,libero
	asiento asiento;
} typedef asientoPedido;

struct {
	long mtype;
	int tipoMensaje;
	union {
		login l;
		loginResponse lresponse;
		pedirSalas salaP;
		elegirSala salaE;
		asientoSeleccionado asientoS;
		finalizarCompra fCompra;

		interaccionSala sala;
		asientoPedido aPedido;

	};
} typedef mensaje;

#endif
