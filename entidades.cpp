/*
 * entidades.cpp
 *
 *  Created on: Sep 12, 2017
 *      Author: tobias
 */

#include "entidades.h"

bool operator==(struct asiento& a, struct asiento& b)
{
	return a.x == b.x && a.y == b.y;
}


