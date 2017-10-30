/* 
 * serializador.cpp
 *
 *  Created on: Oct 16, 2017
 *      Author: tobias
 */

#include "serializador.h"
#include <string>
#include <sstream>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>



static std::string validarBarraCero(const char* campo)
{
	unsigned int longitud = strnlen(campo, LONGITUDMAXNOMBRE);
	char campoValido[LONGITUDMAXNOMBRE];
	strncpy(campoValido, campo, longitud);

	if(longitud < LONGITUDMAXNOMBRE)
		campoValido[longitud] = '\0';
	else
		campoValido[LONGITUDMAXNOMBRE-1] = '\0';

	return std::string("");
}

static std::string serializarEstadoAsiento(const struct reserva &res)
{
	std::stringstream serial;

	//Aseguro que este el '\0' en el nombre del cliente.
	std::string nombreCliente = validarBarraCero(res.nombreCliente);
	
	serial << res.idSala << SEPARADOR << res.idUsuario << SEPARADOR << nombreCliente << SEPARADOR << (int)res.estado << SEPARADOR << res.asiento.x << SEPARADOR << res.asiento.y;
	
	std::string aux;
	getline(serial, aux);
	return aux;
} 

static std::string serializarInformacionSala(const struct sala &InformacionSala)
{
	std::stringstream serial;

	//Serializo los asientos.
	for(unsigned int i=0; i<CANTIDADMAXASIENTOS; i++)
		for(unsigned int a=0; a<CANTIDADMAXASIENTOS; a++)
			serial << serializarEstadoAsiento(InformacionSala.estadoAsientos[i][a]) << SEPARADOR;

	//Serializo el id.
	serial << InformacionSala.id;

	std::string aux;
	getline(serial, aux);
	return aux;
}



bool serializar(const mensaje &msg, std::string &serializado, bool peticion){
	std::stringstream serial;

	//Serializa la primera parte del mensaje, comun a todos los mensajes.
	serial << msg.mtype << SEPARADOR << msg.tipoMensaje << SEPARADOR << (int)msg.resultado << SEPARADOR << msg.fd << SEPARADOR;

	//Serializa dependiendo del tipo de mensaje.
	std::string nombreCliente, password;

	switch(msg.tipoMensaje)
	{
		case LOGIN:
			//Aseguro que este el '\0' en el nombre del cliente y la contrasena.
			nombreCliente = validarBarraCero(msg.l.user.nombreCliente);
			password = validarBarraCero(msg.l.user.password);

			serial << msg.l.id << SEPARADOR << nombreCliente << SEPARADOR << password;
			break;
		case LOGIN_RESPONSE:
			serial << msg.lresponse.success;
			break;
		case PEDIR_SALAS:
			serial << msg.salaP.salas.totalSalas;

			for(unsigned int i=0;i<CANTIDADMAXSALAS; i++)
 				serial << SEPARADOR << msg.salaP.salas.numeros[i];
			break;
		case ELEGIR_SALA:
			if(peticion)
				serial << msg.salaE.userid << SEPARADOR << msg.salaE.salaid;
			else
				serial << serializarInformacionSala(msg.informacionSala);
			
			break;
		case INTERACCION_ASIENTO:
			serial << serializarEstadoAsiento(msg.asientoS);
			break;
		case FINALIZAR_COMPRA:
			serial << msg.fCompra.userid;
			break;
		case TIMEOUT:
			break;
		case SALIR_SALA:
			break;
		case AVISOASINCRONO:
			serial << serializarInformacionSala(msg.informacionSala);
			break;
		case INITMOM:
			//Es local.
			break;
		case DESTROY_MOM:
			//Es local.
			break;
		case ACTUALIZAR_ASIENTOS:
			//Es local.
			break;
		default:
			return false;
	}

	getline(serial, serializado);
	return true;
}

bool serializar(const int numero, std::string &serializado)
{
	std::stringstream serial;
	serial << numero;
	std::getline(serial, serializado);
	return true;
}

static std::vector<std::string> cortarString(const std::string &serial)
{
	std::vector<std::string> partes;
	unsigned int a=0, i;

	for(i = 0; i<serial.length(); i++)
	{
		if(serial.at(i) == SEPARADOR)
		{
			partes.push_back(serial.substr(a, i - a));
			a = i + 1;	
		}		
	}


	if(a < i)
	{
		partes.push_back(serial.substr(a));
	} else if( serial.at(serial.length() -1 ) == SEPARADOR )
	{
		partes.push_back("");
	}



	return partes;
}

static long aLong(std::string numero)
{
	return strtol(numero.c_str(), NULL, 10);
}

static bool desserializarInformacionSala(struct sala &informacionSala, std::vector<std::string> &partes, unsigned int indiceInicial)
{
	for(unsigned int i=0; i<CANTIDADMAXASIENTOS; i++)
		for(unsigned int a=0; a<CANTIDADMAXASIENTOS; a++)
		{
			informacionSala.estadoAsientos[i][a].idSala = aLong(partes[indiceInicial++]);
			informacionSala.estadoAsientos[i][a].idUsuario = aLong(partes[indiceInicial++]);
			strncpy(informacionSala.estadoAsientos[i][a].nombreCliente, partes[indiceInicial++].c_str(), LONGITUDMAXNOMBRE);
			informacionSala.estadoAsientos[i][a].estado = (char)aLong(partes[indiceInicial++]);
			informacionSala.estadoAsientos[i][a].asiento.x = aLong(partes[indiceInicial++]);
			informacionSala.estadoAsientos[i][a].asiento.y = aLong(partes[indiceInicial++]);  
		} 
			 
	

	informacionSala.id = aLong(partes[indiceInicial++]); 
	return true;
}


bool desserializar(std::string &serializado, mensaje &desserializado, bool peticion)
{
	std::vector<std::string> valores = cortarString(serializado);
	desserializado.mtype = aLong(valores[0]);
	desserializado.tipoMensaje = aLong(valores[1]);
	desserializado.resultado = (char)aLong(valores[2]);
	desserializado.fd = aLong(valores[3]);

	//Proceso inverso al serializado.

	switch(desserializado.tipoMensaje)
	{
		case LOGIN:
			desserializado.l.id = aLong(valores[4]);
			strncpy(desserializado.l.user.nombreCliente, valores[5].c_str(), LONGITUDMAXNOMBRE);
			strncpy(desserializado.l.user.password, valores[6].c_str(), LONGITUDMAXNOMBRE);
			break;
		case LOGIN_RESPONSE:
			desserializado.lresponse.success = aLong(valores[4]);
			break;
		case PEDIR_SALAS:
			desserializado.salaP.salas.totalSalas = aLong(valores[4]);
			
			for(unsigned int i=0;i<CANTIDADMAXSALAS; i++)
 				desserializado.salaP.salas.numeros[i] = aLong(valores[5 + i]);
			break;
		case ELEGIR_SALA:
			if(peticion)
			{
				desserializado.salaE.userid = aLong(valores[4]);
				desserializado.salaE.salaid = aLong(valores[5]);
			}
			else
				desserializarInformacionSala(desserializado.informacionSala, valores, 4);
			
			break;
		case INTERACCION_ASIENTO:
			desserializado.asientoS.idSala = aLong(valores[4]);
			desserializado.asientoS.idUsuario = aLong(valores[5]);
			strncpy(desserializado.asientoS.nombreCliente, valores[6].c_str(), LONGITUDMAXNOMBRE);
			desserializado.asientoS.estado = valores[7].at(0);
			desserializado.asientoS.asiento.x = aLong(valores[8]);
			desserializado.asientoS.asiento.y = aLong(valores[9]); 
			break;
		case FINALIZAR_COMPRA:
			desserializado.fCompra.userid = aLong(valores[4]);
			break;
		case TIMEOUT:
			break;
		case SALIR_SALA:
			break;
		case AVISOASINCRONO:
			desserializarInformacionSala(desserializado.informacionSala, valores, 4);
			break;
		case INITMOM:
			//Es local.
			break;
		case DESTROY_MOM:
			//Es local.
			break;
		case ACTUALIZAR_ASIENTOS:
			//Es local.
			break;
		default:
			return false;
	}
	return true;
}

bool desserializar(std::string &serializado, int &numero)
{
	numero = aLong(serializado);
	return true;
}


