#ifndef BASEDEDATOS_H_
#define BASEDEDATOS_H_
#include "../entidades.h"
#include <vector>

int loginUser(user user);

std::vector<sala> obtenerNombresSalas();

asientos obtenerAsientos(sala sala);


#endif /* BASEDEDATOS_H_ */
