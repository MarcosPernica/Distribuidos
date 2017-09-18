/*
 * timeout.h
 *
 *  Created on: Sep 17, 2017
 *      Author: tobias
 */

#ifndef TIMEOUT_H_
#define TIMEOUT_H_

#define TIME_OUT_USER 30
#define TIME_OUT_CONNECTION 10

#include <time.h>
#include<errno.h>

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
