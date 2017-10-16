/*
 * paramsParser.h
 *
 *  Created on: Oct 16, 2017
 *      Author: tobias
 */

#ifndef PARAMSPARSER_H_
#define PARAMSPARSER_H_

/* Devuelve True si pudo parsear un ip, false en otro caso
 * No verifica que argv tenga el indice indicado */
bool parseIp(char* argv, char* ip);

/* Devuelve True si pudo parsear un puerto valido, false en otro caso
 * No verifica que argv tenga el indice indicado */
bool parsePort(char* argv, int* port);

#endif /* PARAMSPARSER_H_ */
