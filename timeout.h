/*
 * timeout.h
 *
 *  Created on: Sep 17, 2017
 *      Author: tobias
 */

#ifndef TIMEOUT_H_
#define TIMEOUT_H_

#define TIME_OUT_USER 120
#define TIME_OUT_CONNECTION 350

#include <time.h>
#include <errno.h>
#include <unistd.h>

void ponerAlarma(int tiempo){
	alarm(tiempo);
}

void cancelarAlarma(){
	alarm(0);
}

bool ocurrioAlarma(int &alarma){
	if( errno == EINTR){
		if ( alarma ){
			alarma = 0;
			return true;
		}
	}
	return false;
}

#endif /* TIMEOUT_H_ */
