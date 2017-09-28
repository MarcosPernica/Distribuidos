#ifndef _ENTIDADES_
#define _ENTIDADES_

#define CANTIDADMAXASIENTOS 10
#define CANTIDADMAXSALAS 20
#define LONGITUDMAXNOMBRE 20

#define ESTADOASIENTOLIBRE 'L'
#define ESTADOASIENTORESERVADO 'R'
#define ESTADOASIENTOCOMPRADO 'C'

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
	int idUsuario;
	char nombreCliente[LONGITUDMAXNOMBRE];
	char estado;
	struct asiento asiento;
};

//Me di cuenta de que el campo cantidad de asientos se podia eliminar y entonces 'asientos' era simplemente 'reserva[CANTIDADMAXASIENTOS][CANTIDADMAXASIENTOS]'
typedef reserva asientos[CANTIDADMAXASIENTOS][CANTIDADMAXASIENTOS];

void copiarAsientos(asientos &destino, const asientos &origen);

struct sala{
	asientos estadoAsientos;
	int id;
};

struct salas{
	int totalSalas;
	int numeros[CANTIDADMAXSALAS];
};


#endif
