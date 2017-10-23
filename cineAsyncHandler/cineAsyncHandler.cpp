#include "cineAsyncHandler.h"
#include <string.h>

//TODO validacion tamanos

int addresIn(struct socketMapper * map, char* address, int addressLen)
{
	for( int i = 0; i < map->amountAddress; i++){
		struct addressMap add = map->addresses[i];
		if( strcmp(add.address, address) == 0 ){
			return i;
		}
	}
	return -1;
}

int clientIn(struct addressMap *toSwap, int client)
{
	for( int i = 0; i < toSwap->amountClients; i++){
		if( client == toSwap->clients[i] ){
			return i;
		}
	}
	return -1;
}

bool addAddress(struct socketMapper * map, char* address, int addressLen, int port){
	if( addresIn(map, address, addressLen) == -1)
	{
		struct addressMap addressmap;
		memcpy(addressmap.address, address, addressLen);
		addressmap.port = port;
		map->addresses[map->amountAddress] = addressmap;
		map->amountAddress++;
		return true;
	}
	return false;
}

bool removeAddress(struct socketMapper * map, char* address, int addressLen)
{
	int index;
	if( (index = addresIn(map, address, addressLen)) != -1 )
	{
		struct addressMap toSwap = map->addresses[map->amountAddress - 1];
		map->addresses[index] = toSwap;
		map->amountAddress--;
		return true;
	}
	return false;
}

bool addClient(struct socketMapper * map,char* address, int addressLen, int client){
	int index;
	if( ( index = addresIn(map, address, addressLen) ) != -1)
	{
		struct addressMap toSwap = map->addresses[index];
		if( clientIn(&toSwap,client) == -1 ){
			toSwap.clients[toSwap.amountClients] = client;
			toSwap.amountClients++;
			map->addresses[index] = toSwap;
			return true;
		}
	}
	return false;
}

bool removeClient(struct socketMapper * map,char* address, int addressLen, int client){
	int index;
	if( ( index = addresIn(map, address, addressLen) ) != -1)
	{
		struct addressMap toSwap = map->addresses[index];
		int cliIndex;
		if( ( cliIndex = clientIn(&toSwap,client) ) != -1 ){
			toSwap.amountClients--;
			toSwap.clients[cliIndex] = toSwap.clients[toSwap.amountClients];
			map->addresses[index] = toSwap;
			return true;
		}
	}
	return false;
}

std::string getClientAddress(struct socketMapper * map, int client, int *port)
{
	int cliIndex;
	for( int i = 0; i < map->amountAddress; i++){
		struct addressMap add = map->addresses[i];
		if( (cliIndex = clientIn(&add,client)) != -1 )
		{
			*port = add.port;
			return std::string(add.address);
		}
	}
	return "";
}
