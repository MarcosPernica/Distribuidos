#ifndef ENTIDADES
#define ENTIDADES

struct user {
	char nombre[20];
	char password[20];
};

struct asiento{
	int x;
	int y;
	int length;
	char nombre[4];
};

struct asientos {
	int totalAsientos;
	asiento asientos[10];
};

struct sala{
	int length;
	char sala[20];
};

struct salas{
	int totalSalas;
	sala salas[20];
};


#endif
