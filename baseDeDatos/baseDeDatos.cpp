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
				aux.idSala = i;
				aux.nombreCliente[0] = '\n';
				aux.estado = ESTADOASIENTOLIBRE;
				aux.asiento.x = a;
				aux.asiento.y = e;

				db.estadoAsientosEnSalas[i][a][e] = aux; 
			}

	return true;
}	

bool dbEntrarSala(struct db &db, struct elegirSala consulta)
{	
	auto sala = db.salaPorUsuario.find(consulta.userid);

	if(sala == db.salaPorUsuario.end() || sala->second == consulta.salaid)
	{
		db.usuariosPorSala.insert(std::pair<unsigned int, unsigned int>(consulta.salaid, consulta.userid));
		db.salaPorUsuario[consulta.userid] = consulta.salaid;	
		return true;
	}
	return false;
}

bool dbSalirSala(struct db &db, struct elegirSala consulta)
{
	auto sala = db.salaPorUsuario.find(consulta.userid);

	if(sala != db.salaPorUsuario.end())
	{
		auto usuarios = db.usuariosPorSala.equal_range(sala->second);
		
		for(auto i = usuarios.first; i != usuarios.second; i++)
			if(consulta.userid == i->second)
			{
				db.salaPorUsuario.erase(i);
				break;
			}
		db.salaPorUsuario.erase(consulta.userid);
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
	if(reserva.idSala < db.cantidadSalas && reserva.asiento.x < CANTIDADMAXASIENTOS, reserva.asiento.y < CANTIDADMAXASIENTOS)
	{
		db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.y][reserva.asiento.x] = reserva;
		db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.y][reserva.asiento.x].estado = ESTADOASIENTORESERVADO;
		return  true;
	}
	return false;	
}

bool dbComprarAsiento(struct db &db, const struct reserva &reserva)
{
	if(reserva.idSala < db.cantidadSalas && reserva.asiento.x < CANTIDADMAXASIENTOS, reserva.asiento.y < CANTIDADMAXASIENTOS)
	{
		db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.y][reserva.asiento.x] = reserva;
		db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.y][reserva.asiento.x].estado = ESTADOASIENTOCOMPRADO;
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
