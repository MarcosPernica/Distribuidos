#ifndef ENTIDADES
#define ENTIDADES

#define CANTIDADMAXASIENTOS 10
#define CANTIDADMAXSALAS 20
#define LONGITUDMAXNOMBRE 20

struct user {
	char nombre[LONGITUDMAXNOMBRE];
	char password[LONGITUDMAXNOMBRE];
};

struct reserva
{
	char nombre[LONGITUDMAXNOMBRE];
	char estado;
	asiento asiento;
};

struct asiento{
	int x;
	int y;
};

struct asientos {
	int totalAsientos;
	reserva reservas[CANTIDADMAXASIENTOS];
};

struct sala{
	asientos asientos;
	int id;
};

struct salas{
	int totalSalas;
	int numero[CANTIDADMAXSALAS];
};


#endif
