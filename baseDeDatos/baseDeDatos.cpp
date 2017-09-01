#include "baseDeDatos.h"
#include <string>
int loginUser(user user){
	//TODO
	return 1;
}

std::vector<sala> obtenerNombresSalas(){
	std::vector<sala> salas;
	for (int i = 0; i < 5; i++){
		sala sala;
		sala.length = 5;
		std::string nombre = "holaa";
		int j;
		for ( j = 0; j < nombre.length(); j++){
			sala.sala[j] = nombre.at(j);
		}
		sala.sala[j] = '\0';
		salas.push_back(sala);
	}
	return salas;
}

asientos obtenerAsientos(sala sala){
	asientos asientos;
	asientos.totalAsientos = 8;
	for (int i = 0; i < asientos.totalAsientos; i++){
		asiento lugar;
		std::string nombre = "F1";
		lugar.length = nombre.length();
		int j;
		for ( j = 0; j < nombre.length(); j++){
			lugar.nombre[j] = nombre.at(j);
		}
		lugar.nombre[j] = '\0';
		lugar.x = 20;
		lugar.y = 15;
		asientos.asientos[i] = lugar;
	}
	return asientos;
}
