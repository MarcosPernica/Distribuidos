#include "baseDeDatos.h"
#include <string>
#include "../entidades.h"

bool dbCrear(struct db &db, unsigned int cantidadSalas)
{
	db.cantidadSalas = cantidadSalas;

	for(unsigned int i = 0; i < cantidadSalas;  i++)
		for(unsigned int a = 0; a < CANTIDADMAXASIENTOS; a++)
			for(unsigned int e = 0; e < CANTIDADMAXASIENTOS; e++)
			{
				struct reserva aux;
				aux.idUsuario = 0;
				aux.idSala = i;
				aux.nombreCliente[0] = '\n';
				aux.estado = ESTADOASIENTOLIBRE;
				aux.asiento.x = a;
				aux.asiento.y = e;

				db.estadoAsientosEnSalas[i][a][e] = aux; 
			}

	return true;
}	

bool dbEntrarSala(struct db &db, int idUsuario, int idSala)
{	
	auto sala = db.salaPorUsuario.find(idUsuario);

	if(sala == db.salaPorUsuario.end() || sala->second == idUsuario)
	{
		db.usuariosPorSala.insert(std::pair<unsigned int, unsigned int>(idSala, idUsuario));
		db.salaPorUsuario[idUsuario] = idSala;	
		return true;
	}
	return false;
}

bool dbSalirSala(struct db &db, int idUsuario)
{
	auto sala = db.salaPorUsuario.find(idUsuario);

	if(sala != db.salaPorUsuario.end())
	{
		auto usuarios = db.usuariosPorSala.equal_range(sala->second);
		
		for(auto i = usuarios.first; i != usuarios.second; i++){
			if( idUsuario == i->second )
			{
				db.usuariosPorSala.erase(i);
				break;
			}
		}
		db.salaPorUsuario.erase(idUsuario);
	}
	return true;
}

std::vector<unsigned int> dbObtenerUsuariosSala(const struct db &db, unsigned int idSala)
{
	std::vector<unsigned int> resultados;

	auto valores = db.usuariosPorSala.equal_range(idSala);
	for(auto i = valores.first;i != valores.second;i++)
		resultados.push_back(i->second);
	return resultados;
}


bool dbReservarAsiento(struct db &db, const struct reserva &reserva)
{
	if(reserva.idSala < db.cantidadSalas && reserva.asiento.x < CANTIDADMAXASIENTOS && reserva.asiento.y < CANTIDADMAXASIENTOS)
	{
		if(db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.x][reserva.asiento.y].estado == ESTADOASIENTOLIBRE)
		{
			db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.x][reserva.asiento.y] = reserva;
			db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.x][reserva.asiento.y].estado = ESTADOASIENTORESERVADO;
			return  true;
		}
		return false;
	}
	return false;	
}


int dbObtenerSalaDeUsuario(const struct db &db,int userid){
	auto sala = db.salaPorUsuario.find(userid);
	if( sala != db.salaPorUsuario.end() )
	{
		return sala->second;
	}
	return -1;
}

bool dbComprarAsientos(struct db &db, const struct finalizarCompra &compra)
{
	auto sala = db.salaPorUsuario.find(compra.userid);

	if( sala != db.salaPorUsuario.end() )
	{
		for(unsigned int i=0;i < CANTIDADMAXASIENTOS; i++)
			for(unsigned int a = 0; a < CANTIDADMAXASIENTOS; a++)
			{
				struct reserva aux = db.estadoAsientosEnSalas[sala->second][i][a];
				if( aux.idUsuario == compra.userid && aux.estado == ESTADOASIENTORESERVADO )
				{
					aux.estado = ESTADOASIENTOCOMPRADO;
					db.estadoAsientosEnSalas[sala->second][i][a] = aux;
				}
			}
		return true;
	}
	return false;	
}

bool dbLiberarAsiento(struct db &db, const struct reserva &reserva)
{
	if(reserva.idSala < db.cantidadSalas && reserva.asiento.x < CANTIDADMAXASIENTOS, reserva.asiento.y < CANTIDADMAXASIENTOS)
	{
		db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.y][reserva.asiento.x].estado = ESTADOASIENTOLIBRE;
		return true;
	}
	return false;
}

bool dbLiberarAsientos(struct db &db, int idUsuario)
{
	auto sala = db.salaPorUsuario.find(idUsuario);

	if(sala != db.salaPorUsuario.end())
	{
		for(unsigned int i=0;i < CANTIDADMAXASIENTOS; i++)
			for(unsigned int a = 0; a < CANTIDADMAXASIENTOS; a++)
			{
				struct reserva aux = db.estadoAsientosEnSalas[sala->second][i][a];
				if(aux.idUsuario == idUsuario && aux.estado == ESTADOASIENTORESERVADO)
				{
					aux.estado = ESTADOASIENTOLIBRE;
					db.estadoAsientosEnSalas[sala->second][i][a] = aux;
				}
			}
	}
	return true;	
}

struct salas dbObtenerSalas(const struct db &db)
{
	struct salas salas;
	salas.totalSalas = db.cantidadSalas;

	for(unsigned int i = 0; i < db.cantidadSalas; i++)
		salas.numeros[i] = i;

	return salas;
}

const asientos &dbObtenerAsientosSala(const struct db &db, unsigned int idSala)
{
	return db.estadoAsientosEnSalas[idSala]; 

}
