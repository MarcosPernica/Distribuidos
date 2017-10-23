/*
 * cineAsyncHandler.h
 *
 *  Created on: Oct 19, 2017
 *      Author: tobias
 */

#ifndef CINEASYNCHANDLER_H_
#define CINEASYNCHANDLER_H_
#include "common.h"

struct addressMap
{
	int amountClients = 0;
	char address[MAX_LEN_ADDRESS];
	int port = -1;
	int clients[MAX_AMOUNT_CLIENTS_PER_SOCKET];
};

struct socketMapper
{
	int amountAddress = 0;
	struct addressMap addresses[100];
};

//Adds an address to the structure if it not exists already
bool addAddress(struct socketMapper * map, char* address, int addressLen, int port);

//Removes an address to the structure if it not exists already
bool removeAddress(struct socketMapper * map, char* address, int addressLen);

//Adds to the address of the structure the client if it not exists already
bool addClient(struct socketMapper * map, char* address, int addressLen, int client);

//Removes the client from the address to the structure
bool removeClient(struct socketMapper * map,char* address, int addressLen, int client);

//Gets the address for the client, returns null if it doesnt exist
std::string getClientAddress(struct socketMapper * map, int client, int *port);



#endif /* CINEASYNCHANDLER_H_ */
