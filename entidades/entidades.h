#ifndef ENTIDADES
#define ENTIDADES

struct {
	char nombre[20];
	char password[20];
} typedef user;

struct {
	int x;
	int y;
	int length;
	char nombre[4];
} asiento;

struct {
	int totalAsientos;
	asiento asientos[10];
} asientos;

struct {
	int length;
	char sala[20];
} typedef sala;

struct {
	int totalSalas;
	sala salas[20];
} typedef salas;


#endif
