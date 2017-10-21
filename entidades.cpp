/*
 * entidades.cpp
 *
 *  Created on: Sep 12, 2017
 *      Author: tobias
 */

#include "entidades.h"
#include <stdio.h>

bool operator==(struct asiento& a, struct asiento& b)
{
	return a.x == b.x && a.y == b.y;
}

void copiarAsientos(asientos &destino, const asientos &origen)
{
	for(unsigned int i = 0; i< CANTIDADMAXASIENTOS; i++)
		for(unsigned int a = 0; a < CANTIDADMAXASIENTOS; a++)
			destino[i][a] = origen[i][a];
}

