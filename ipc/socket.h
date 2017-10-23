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

int crearSocketCliente(struct sockaddr address, int len);

int crearSocketServer(int port, int backlog);

int leerSocketEntero(int fd, char * buffer, int maxLen);

int escribirSocketEntero(int fd, char * buffer, int len);

int nextIndexOf(char character, char* buffer, int fromIndex, int len );

int leerSocketHasta(int fd, char* buffer, int maxLen, char endLine, int &firstEndLine);

#endif /* SOCKET_H_ */
