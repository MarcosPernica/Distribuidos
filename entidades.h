#ifndef ENTIDADES
#define ENTIDADES

#define CANTIDADMAXASIENTOS 10
#define CANTIDADMAXSALAS 20
#define LONGITUDMAXNOMBRE 20

struct user {
	char nombre[LONGITUDMAXNOMBRE];
	char password[LONGITUDMAXNOMBRE];
};

struct asiento{
	int x;
	int y;
	int length;
	char nombre[LONGITUDMAXNOMBRE];
};

struct asientos {
	int totalAsientos;
	asiento asientos[CANTIDADMAXASIENTOS];
};

struct sala{
	int length;
	char sala[LONGITUDMAXNOMBRE];
};

struct salas{
	int totalSalas;
	sala salas[CANTIDADMAXSALAS];
};


#endif
