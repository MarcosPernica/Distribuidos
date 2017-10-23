#include "serializador.h"
#include "mensajes.h"
#include "entidades.h"
#include <iostream>
#include <string>
#include <string.h>


using namespace std;

int main()
{
	mensaje m, m2;
	bool test;

	m.mtype = 10;
	m.tipoMensaje = AVISOASINCRONO;
	m.resultado = 3;
	m.fd = 2147483647;

	for(unsigned int i=0; i<CANTIDADMAXASIENTOS; i++)
		for(unsigned int a=0; a<CANTIDADMAXASIENTOS; a++)
		{
			m.informacionSala.estadoAsientos[i][a].idSala = i;
			m.informacionSala.estadoAsientos[i][a].idUsuario = i*a;
			strncpy(m.informacionSala.estadoAsientos[i][a].nombreCliente, "Soy Marcos", LONGITUDMAXNOMBRE);
			m.informacionSala.estadoAsientos[i][a].estado = ESTADOASIENTOLIBRE;
			m.informacionSala.estadoAsientos[i][a].asiento.x = a;
			m.informacionSala.estadoAsientos[i][a].asiento.y = i;  
		} 

	m.informacionSala.estadoAsientos[1][5].idSala = 8;
	m.informacionSala.estadoAsientos[1][5].idUsuario = 17;
	strncpy(m.informacionSala.estadoAsientos[1][5].nombreCliente, "Nariz", LONGITUDMAXNOMBRE);
	m.informacionSala.estadoAsientos[1][5].estado = ESTADOASIENTORESERVADO;
	m.informacionSala.estadoAsientos[1][5].asiento.x = 1;
	m.informacionSala.estadoAsientos[1][5].asiento.y = 5;

	m.informacionSala.id = 118;
	
	string serial;

	serializar(m, serial);

	desserializar(serial, m2);

	test = true;

	test &= m.mtype == m2.mtype;
	test &= m.tipoMensaje == m2.tipoMensaje;
	test &= m.resultado == m2.resultado;
	test &= m.fd == m2.fd;

	for(unsigned int i=0; i<CANTIDADMAXASIENTOS; i++)
		for(unsigned int a=0; a<CANTIDADMAXASIENTOS; a++)
		{
			test &= m.informacionSala.estadoAsientos[i][a].idSala == m2.informacionSala.estadoAsientos[i][a].idSala;
			test &= m.informacionSala.estadoAsientos[i][a].idUsuario == m2.informacionSala.estadoAsientos[i][a].idUsuario;
			test &= (strncmp(m.informacionSala.estadoAsientos[i][a].nombreCliente, m2.informacionSala.estadoAsientos[i][a].nombreCliente, LONGITUDMAXNOMBRE)==0);
			test &= m.informacionSala.estadoAsientos[i][a].estado == m2.informacionSala.estadoAsientos[i][a].estado;
			m.informacionSala.estadoAsientos[i][a].asiento.x == m2.informacionSala.estadoAsientos[i][a].asiento.x;
			m.informacionSala.estadoAsientos[i][a].asiento.y == m2.informacionSala.estadoAsientos[i][a].asiento.y;  
		} 


	test &= m.informacionSala.id == m2.informacionSala.id;


	cout << "Resultado del test de igualdad de estado de sala: " << std::boolalpha << test << endl;


	m.mtype = 10;
	m.tipoMensaje = AVISOASINCRONO;
	m.resultado = 3;
	m.fd = 2147483647;
	
	serial;

	serializar(m, serial);

	desserializar(serial, m2);

	test = true;

	test &= m.mtype == m2.mtype;
	test &= m.tipoMensaje == m2.tipoMensaje;
	test &= m.resultado == m2.resultado;
	test &= m.fd == m2.fd;

	for(unsigned int i=0; i<CANTIDADMAXASIENTOS; i++)
		for(unsigned int a=0; a<CANTIDADMAXASIENTOS; a++)
		{
			test &= m.informacionSala.estadoAsientos[i][a].idSala == m2.informacionSala.estadoAsientos[i][a].idSala;
			test &= m.informacionSala.estadoAsientos[i][a].idUsuario == m2.informacionSala.estadoAsientos[i][a].idUsuario;
			test &= (strncmp(m.informacionSala.estadoAsientos[i][a].nombreCliente, m2.informacionSala.estadoAsientos[i][a].nombreCliente, LONGITUDMAXNOMBRE)==0);
			test &= m.informacionSala.estadoAsientos[i][a].estado == m2.informacionSala.estadoAsientos[i][a].estado;
			m.informacionSala.estadoAsientos[i][a].asiento.x == m2.informacionSala.estadoAsientos[i][a].asiento.x;
			m.informacionSala.estadoAsientos[i][a].asiento.y == m2.informacionSala.estadoAsientos[i][a].asiento.y;  
		} 


	test &= m.informacionSala.id == m2.informacionSala.id;


	cout << "Resultado del test de igualdad de estado de sala con campos basura: " << std::boolalpha << test << endl;

	m.mtype = 101;
	m.tipoMensaje = LOGIN;
	m.resultado = 14;
	m.fd = 48;

	m.l.id = 141;
	strncpy(m.l.user.nombreCliente,"Marcos",LONGITUDMAXNOMBRE);
	strncpy(m.l.user.password,"uirguirkgh",LONGITUDMAXNOMBRE);
	
	serial;

	serializar(m, serial);

	desserializar(serial, m2);

	test = true;

	test &= m.mtype == m2.mtype;
	test &= m.tipoMensaje == m2.tipoMensaje;
	test &= m.resultado == m2.resultado;
	test &= m.fd == m2.fd;

	test &= m.l.id == m2.l.id;
	test &= (strncmp(m.l.user.nombreCliente, m2.l.user.nombreCliente, LONGITUDMAXNOMBRE) == 0);
	test &= (strncmp(m.l.user.password, m2.l.user.password, LONGITUDMAXNOMBRE) == 0);


	cout << "Resultado del test de igualdad de login: " << std::boolalpha << test << endl;
	
	serial;

	serializar(m, serial);

	desserializar(serial, m2);

	test = true;

	test &= m.mtype == m2.mtype;
	test &= m.tipoMensaje == m2.tipoMensaje;
	test &= m.resultado == m2.resultado;
	test &= m.fd == m2.fd;

	test &= m.l.id == m2.l.id;
	test &= (strncmp(m.l.user.nombreCliente, m2.l.user.nombreCliente, LONGITUDMAXNOMBRE) == 0);
	test &= (strncmp(m.l.user.password, m2.l.user.password, LONGITUDMAXNOMBRE) == 0);


	cout << "Resultado del test de igualdad de login con datos basura: " << std::boolalpha << test << endl;



	m.mtype = 101;
	m.tipoMensaje = LOGIN_RESPONSE;
	m.resultado = 14;
	m.fd = 48;

	m.lresponse.success = 1234;
	
	serial;

	serializar(m, serial);

	desserializar(serial, m2);

	test = true;

	test &= m.mtype == m2.mtype;
	test &= m.tipoMensaje == m2.tipoMensaje;
	test &= m.resultado == m2.resultado;
	test &= m.fd == m2.fd;

	test &= m.lresponse.success == m2.lresponse.success;


	cout << "Resultado del test de igualdad de login response: " << std::boolalpha << test << endl;


	m.tipoMensaje = PEDIR_SALAS;
	
	serial;

	serializar(m, serial);
	desserializar(serial, m2);

	test = true;

	test &= m.mtype == m2.mtype;
	test &= m.tipoMensaje == m2.tipoMensaje;
	test &= m.resultado == m2.resultado;
	test &= m.fd == m2.fd;

	test &= m.salaP.salas.totalSalas == m2.salaP.salas.totalSalas;

	for(unsigned int i=0;i<CANTIDADMAXSALAS;i++)
		test &= m.salaP.salas.numeros[i] == m2.salaP.salas.numeros[i];


	cout << "Resultado del test de igualdad de pedir salas: " << std::boolalpha << test << endl;

	m.tipoMensaje = ELEGIR_SALA;
	
	serial;

	serializar(m, serial);
	desserializar(serial, m2);

	test = true;

	test &= m.mtype == m2.mtype;
	test &= m.tipoMensaje == m2.tipoMensaje;
	test &= m.resultado == m2.resultado;
	test &= m.fd == m2.fd;

	test &= m.salaE.userid == m2.salaE.userid;
	test &= m.salaE.salaid == m2.salaE.salaid;
	


	cout << "Resultado del test de igualdad de elegir sala [request]: " << std::boolalpha << test << endl;

	m.tipoMensaje = ELEGIR_SALA;
	
	serial;

	serializar(m, serial, false);
	desserializar(serial, m2, false);

	test = true;

	test &= m.mtype == m2.mtype;
	test &= m.tipoMensaje == m2.tipoMensaje;
	test &= m.resultado == m2.resultado;
	test &= m.fd == m2.fd;

	for(unsigned int i=0; i<CANTIDADMAXASIENTOS; i++)
		for(unsigned int a=0; a<CANTIDADMAXASIENTOS; a++)
		{
			test &= m.informacionSala.estadoAsientos[i][a].idSala == m2.informacionSala.estadoAsientos[i][a].idSala;
			test &= m.informacionSala.estadoAsientos[i][a].idUsuario == m2.informacionSala.estadoAsientos[i][a].idUsuario;
			test &= (strncmp(m.informacionSala.estadoAsientos[i][a].nombreCliente, m2.informacionSala.estadoAsientos[i][a].nombreCliente, LONGITUDMAXNOMBRE)==0);
			test &= m.informacionSala.estadoAsientos[i][a].estado == m2.informacionSala.estadoAsientos[i][a].estado;
			m.informacionSala.estadoAsientos[i][a].asiento.x == m2.informacionSala.estadoAsientos[i][a].asiento.x;
			m.informacionSala.estadoAsientos[i][a].asiento.y == m2.informacionSala.estadoAsientos[i][a].asiento.y;  
		} 


	test &= m.informacionSala.id == m2.informacionSala.id;
	


	cout << "Resultado del test de igualdad de elegir sala [response]: " << std::boolalpha << test << endl;



	m.tipoMensaje = INTERACCION_ASIENTO;
	
	serial;

	serializar(m, serial);
	desserializar(serial, m2);

	test = true;

	test &= m.mtype == m2.mtype;
	test &= m.tipoMensaje == m2.tipoMensaje;
	test &= m.resultado == m2.resultado;
	test &= m.fd == m2.fd;

	test &= m.asientoS.idSala == m2.asientoS.idSala;
	test &= m.asientoS.idUsuario == m2.asientoS.idUsuario;
	test &= strncmp(m.asientoS.nombreCliente,m2.asientoS.nombreCliente,LONGITUDMAXNOMBRE)==0;
	test &= m.asientoS.estado == m2.asientoS.estado;
	test &= m.asientoS.asiento.x == m2.asientoS.asiento.x;
	test &= m.asientoS.asiento.y == m2.asientoS.asiento.y;
	

	cout << "Resultado del test de igualdad de interaccion asiento: " << std::boolalpha << test << endl;


	m.tipoMensaje = FINALIZAR_COMPRA;
	
	serial;

	serializar(m, serial);
	desserializar(serial, m2);

	test = true;

	test &= m.mtype == m2.mtype;
	test &= m.tipoMensaje == m2.tipoMensaje;
	test &= m.resultado == m2.resultado;
	test &= m.fd == m2.fd;

	test &= m.fCompra.userid == m2.fCompra.userid;
	
	

	cout << "Resultado del test de igualdad de finalizar compra: " << std::boolalpha << test << endl;

	serial = "";
	int numero, numeroDesserializado;

	serializar(numero, serial);
	desserializar(serial, numeroDesserializado);

	test = numero == numeroDesserializado;
	

	cout << "Resultado del test de igualdad de entero: " << std::boolalpha << test << endl;
 
	return 0;
} 
