/*
 * senal.h
 *
 *  Created on: Sep 4, 2017
 *      Author: tobias
 */

#ifndef SENAL_H_
#define SENAL_H_

#include <signal.h>
#include <stdio.h>
#include <memory.h>

#define EMPTY_MASK_ERROR -2
#define ADD_SIGNAL_ERROR -3
#define ADD_ACTION_ERROR -4

//registra una sola senal
int registrarSenal(int signum, sighandler_t handling);

int enviarSenal(int pid, int signum);

#endif /* SENAL_H_ */
