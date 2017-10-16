/*
 * serializador.cpp
 *
 *  Created on: Oct 16, 2017
 *      Author: tobias
 */

#include "serializador.h"
#include "string.h"

bool serializar(mensaje msg,char* serializado){
	serializado = memcpy(serializado,&msg, sizeof(mensaje));
	return true;
}

bool desserializar(char* msg, int length ,mensaje *deseralizado){
	if( length == sizeof(mensaje) )
	{
		memcpy(deseralizado, msg, length);
		return true;
	}
	return false;
}


