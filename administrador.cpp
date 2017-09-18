#include "stdlib.h"
#include "ipc/cola.h"
#include "ipc/senal.h"
#include "mensajes.h"
#include "common.h"
#include "administrador.h"
#include "baseDeDatos/baseDeDatos.h"
#include "errno.h"

sig_atomic_t admin_vivo = 1;

void terminarAdmin(int sigint){
	admin_vivo = 0;
}

void copiarArregloEnteros(asientos &destino, const asientos &origen)
{
	for(unsigned int i = 0; i< CANTIDADMAXASIENTOS; i++)
		for(unsigned int a = 0; a < CANTIDADMAXASIENTOS; a++)
			destino[i][a] = origen[i][a];
}

bool enviarMensajeAsincronico(int colaEnvioAsincronico, unsigned int idSala, struct db &baseDeDatos)
{
	std::vector<unsigned int> idUsuarios = dbObtenerUsuariosSala(baseDeDatos, idSala);

	//Arma el mensaje.
	mensaje aviso;
	aviso.tipoMensaje = AVISOASINCRONO; 

	struct sala aux;
	copiarArregloEnteros(aux.estadoAsientos, dbObtenerAsientosSala(baseDeDatos, idSala));
	aviso.informacionSala = aux;

	aviso.resultado = RESULTADOOK;

	//Manda uno por usuario.

	for(unsigned int i=0;i<idUsuarios.size();i++)
	{
		//Como tipo de mensaje se usa el id del usuario que lo recibira.
		aviso.mtype = idUsuarios.at(i);
		enviarMensaje(colaEnvioAsincronico,(void*)&aviso,sizeof(aviso));
	}

	return true;
}

void limpiarCola(int colaEnvio, int idUsuario ){
	mensaje recibido;
	bool mensajesEnCola = true;
	while (mensajesEnCola)
	{
		if ( recibirMensajeAsinc(colaEnvio,idUsuario,(void*)&recibido,sizeof(recibido)) == -1 ){
			mensajesEnCola = false;
		}
	}
}


int procesarMensaje(const mensaje &recibido, int colaEnvio, int colaEnvioAsincronico, struct db &baseDeDatos){
	mensaje respuesta;
	respuesta.mtype = recibido.mtype; 	
	respuesta.tipoMensaje = recibido.tipoMensaje;
	respuesta.resultado = RESULTADOOK;

	switch(recibido.tipoMensaje)
		{
			case PEDIR_SALAS:
				respuesta.salaP.salas = dbObtenerSalas(baseDeDatos);
				break;
			case ELEGIR_SALA:
				//Por las dudas sale de la sala si estaba en una.
				dbSalirSala(baseDeDatos, recibido.salaE.userid);
				dbEntrarSala(baseDeDatos, recibido.salaE.userid, recibido.salaE.salaid);
				struct sala aux;
				copiarArregloEnteros(aux.estadoAsientos, dbObtenerAsientosSala(baseDeDatos, recibido.salaE.salaid));
				aux.id = recibido.salaE.salaid;
				respuesta.informacionSala = aux;			
				break;
			case INTERACCION_ASIENTO:
				if( recibido.asientoS.estado == ASIENTORESERVADO )
				{
					if( !dbReservarAsiento(baseDeDatos, recibido.asientoS) )
						respuesta.resultado = RESULTADOERROR;
				}
				else
					dbLiberarAsiento(baseDeDatos, recibido.asientoS);

				//Avisa a los usuarios que se libero/reservo el asiento.
				enviarMensajeAsincronico(colaEnvioAsincronico, recibido.asientoS.idSala, baseDeDatos);
				break;
			case FINALIZAR_COMPRA:

				if( dbComprarAsientos(baseDeDatos, recibido.fCompra) ){
					respuesta.resultado = RESULTADOOK;
					enviarMensajeAsincronico(
							colaEnvioAsincronico,
							dbObtenerSalaDeUsuario(baseDeDatos,recibido.fCompra.userid),
							baseDeDatos);
				}
				else
					respuesta.resultado = RESULTADOERROR;
				break;
			case TIMEOUT: case SALIR_SALA:
				dbLiberarAsientos(baseDeDatos, recibido.idUsuario);
				dbSalirSala(baseDeDatos, recibido.idUsuario);
				limpiarCola(colaEnvioAsincronico,recibido.idUsuario);
				break;
			default:
				respuesta.resultado = RESULTADOCONSULTAERRONEA;
				break;
		}
	return enviarMensaje(colaEnvio,(void*)&respuesta,sizeof(respuesta));
}

void correrAdministrador(){
	//Crea la base de datos vacia.
	struct db baseDeDatos;
	if(!dbCrear(baseDeDatos))
	{
		printf("Admin error al crear base de datos");
		exit(1);
	}

	//Obtiene la cola de recepcion ADMIN_CINEHIJO.
	int colaRecepcion = obtenerCola(COLA_RECEPCION_ADMIN);
	if (colaRecepcion < 0){
		printf("Admin error al obtener cola");
		exit(1);
	}
	//Obtiene la cola de envio ADMIN_CINEHIJO.
	int colaEnvio = obtenerCola(COLA_ENVIO_ADMIN);
	if (colaEnvio < 0){
		printf("Admin error al obtener cola");
		exit(1);
	}
	
	//Obtiene la cola para enviar al cliente los cambios en las salas.
	int colaEnvioAsincronico = obtenerCola(COLA_ASINC_CLIENTE);
	if (colaEnvioAsincronico < 0){
		printf("Admin error al obtener cola");
		exit(1);
	}
	
	//Registra la señal de cerrado del admin.
	int registro = registrarSenal(SIGINT,terminarAdmin);
	if (registro < 0){
		printf("Admin error al obtener cola");
		exit(1);
	}

	mensaje recibido;
	while(admin_vivo){
		int result = recibirMensaje(colaRecepcion,(void*)&recibido,sizeof(recibido));
		if (result == -1){
			printf("Admin error al recibir mensajes\n");
			break;
		}

		if ( procesarMensaje(recibido, colaEnvio, colaEnvioAsincronico, baseDeDatos) < 0){
			printf("Admin error al enviar respuestas");
			break;
		}
	}

	exit(0);
}
