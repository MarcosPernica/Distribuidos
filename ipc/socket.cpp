#include "socket.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <memory.h>
#include <arpa/inet.h>
#include <netdb.h>

int crearSocketCliente(struct sockaddr address, int len){
	int socket_fd;
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return -1;
	}

	if ( connect(socket_fd, &address, len) == -1)
	{
		close(socket_fd);
		return -1;
	}
	return socket_fd;
}

int crearSocketCliente(std::string host, int port){
	struct sockaddr_in serv;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int sock_fd = -1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	char portStr[12];
	sprintf(portStr, "%d", port);

	if ((rv = getaddrinfo(host.c_str(), portStr, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	for (p=servinfo; p != NULL; p = p->ai_next) {
		if ((sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("socket");
			continue;
		}

		if ( connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sock_fd);
			sock_fd = -1;
			perror("connect");
			continue;
		}
		break;
	}
	return sock_fd;
}

int crearSocketServer(int port, int backlog){
	int listen_fd;          //listenning soket

	struct sockaddr_in serv;

	if ( (listen_fd=socket(PF_INET,SOCK_STREAM,0) ) < 0 ){
		perror("socket error! ");
		return -1;
	}

	memset(&serv , 0 , sizeof serv );
	serv.sin_family= AF_INET;
	serv.sin_addr.s_addr= htonl(INADDR_ANY);
	serv.sin_port= htons(port);

	if ( bind(listen_fd, (struct sockaddr *) &serv, sizeof serv  ) < 0  ){
		perror("bind error! ");
		close(listen_fd);
		return -1;
	}

	if ( listen(listen_fd,backlog) <0  ){
		perror("listen error! ");
		close(listen_fd);
		return -1;
	}

	return listen_fd;
}

int leerSocketEntero(int fd, char * buffer, int maxLen){
	int totalLen = 0;
	int len = 0;
	do {
		len = read(fd,buffer + totalLen,maxLen - totalLen );
		totalLen += len;
	}while (len != -1 && totalLen < maxLen );

	if( len < 0 ){
		return len;
	}
	return totalLen;
}

int nextIndexOf(char character, char* buffer, int fromIndex, int len ){
	for( int i = 0; i < len; i++ )
	{
		int index = fromIndex + i;
		if( buffer[index] == character ){
			return index;
		}
	}
	return -1;
}

int leerSocketHasta(int fd, char* buffer, int maxLen, char endLine, int &firstEndLine){
	int totalLen = 0;
	int len = 0;

	do {
		len = read(fd,buffer + totalLen,maxLen - totalLen );
		totalLen += len;
		firstEndLine = nextIndexOf(endLine,buffer, totalLen - len , len);
		if( firstEndLine != -1 ){
			break;
		}
	}while ( len != -1 && totalLen < maxLen );

	if( len < 0 ){
		return len;
	}
	return totalLen;
}

int escribirSocketEntero(int fd, char* buffer, int length){
	int totalLen = 0;
	int len = 0;
	do {
		len = write(fd,buffer + totalLen,length - totalLen );
		totalLen+= len;
	}while (len != -1 && totalLen < length );
	if( len < 0 ){
		return len;
	}
	return totalLen;
}
