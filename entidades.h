#ifndef _ENTIDADES_
#define _ENTIDADES_

#define CANTIDADMAXASIENTOS 10
#define CANTIDADMAXSALAS 20
#define LONGITUDMAXNOMBRE 20

#define ESTADOASIENTOLIBRE 0
#define ESTADOASIENTORESERVADO 1
#define ESTADOASIENTOCOMPRADO 2

struct user {
	char nombreCliente[LONGITUDMAXNOMBRE];
	char password[LONGITUDMAXNOMBRE];
};

struct asiento{
	unsigned int x;
	unsigned int y;
};

bool operator==(struct asiento& a, struct asiento& b);

struct reserva
{
	int idSala;
	char nombreCliente[LONGITUDMAXNOMBRE];
	char estado;
	struct asiento asiento;
};

//Me di cuenta de que el campo cantidad de asientos se podia eliminar y entonces 'asientos' era simplemente 'reserva[CANTIDADMAXASIENTOS][CANTIDADMAXASIENTOS]'
typedef reserva asientos[CANTIDADMAXASIENTOS][CANTIDADMAXASIENTOS];

struct sala{
	asientos estadoAsientos;
	int id;
};

struct salas{
	int totalSalas;
	int numeros[CANTIDADMAXSALAS];
};


#endif
