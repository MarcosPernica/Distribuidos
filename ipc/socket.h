/*
 * socket.h
 *
 *  Created on: Oct 3, 2017
 *      Author: tobias
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <string>

int crearSocketCliente(std::string host, int port);

int crearSocketServer(int port);

int leerSocket(int fd, char * buffer, int maxLen);

int escribirSocket(int fd, const char * buffer, int len);

#endif /* SOCKET_H_ */
