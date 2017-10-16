/*
 * serializador.h
 *
 *  Created on: Oct 16, 2017
 *      Author: tobias
 */

#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_
#include "mensajes.h"

bool serializar(mensaje msg,char* seralizado);

bool desserializar(char* msg,int length,mensaje *deseralizado);

#endif /* SERIALIZADOR_H_ */
