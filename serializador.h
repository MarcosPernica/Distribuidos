/*
 * serializador.h
 *
 *  Created on: Oct 16, 2017
 *      Author: tobias
 */

#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_
#include "mensajes.h"
#include <string>

#define SEPARADOR '|'

//El parametro 'peticion' sirve para determinar si el mensaje es el primero de la interaccion o el segundo(Se usa el mismo tipo de mensaje pero cambia la estructura). 
bool serializar(const mensaje &msg, std::string &serializado, bool peticion = true);

bool desserializar(std::string &serializado, mensaje &desserializado, bool peticion = true);

#endif /* SERIALIZADOR_H_ */
