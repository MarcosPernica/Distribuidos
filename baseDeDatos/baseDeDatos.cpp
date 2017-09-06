#include "baseDeDatos.h"
#include <string>

bool dbCrear(struct db &db, unsigned int cantidadSalas)
{
	db.cantidadSalas = cantidadSalas;

	for(unsigned int i = 0; i < cantidadSalas;  i++)
		for(unsigned int a = 0; a < CANTIDADMAXASIENTOS; a++)
			for(unsigned int e = 0; e < CANTIDADMAXASIENTOS; e++)
				db.estadoAsientosEnSalas[i][a][e] = {.asiento = { .x = a, .y = e}, .idSala = i, .estado = ESTADOASIENTOLIBRE, .nombre[0] = '\n'}; 

	return true;
}

static bool claveEsta(const std::multimap<unsigned int, unsigned int> &mapa, unsigned int clave, unsigned int valor)
{
	auto valores;
	valores = mapa.equal_range(clave);
	for(auto i = valores.first;i != valores.second;i++)
		if(valor == i->second)
			return true;
	return false;
	

bool dbEntrarSala(struct db &db, struct elegirSala)
{
	if(!claveEsta(db.usuariosActivos, elegirSala.idsala, elegirSala.iduser))
	{
		db.usuariosActivos.insert(std::pair<unsigned int, unsigned int>(elegirSala.idsala, elegirSala. iduser))	
		return true;
	}
	return false;
}

bool dbSalirSala(struct db &db, struct elegirSala)
{
	auto valores;
	valores = db.usuariosActivos.equal_range(elegirSala.idsala);
	for(auto i = valores.first;i != valores.second;i++)
		if(elegirSala.iduser == i->second)
		{
			db.usuariosActivos.erase(i);
			return true;
		}
	return false;
}


bool dbReservarAsiento(struct db &db, const struct reserva &reserva)
{
	if(reserva.idSala < cantidadSalas && reserva.asiento.x < CANTIDADMAXASIENTOS, reserva.asiento.y < CANTIDADMAXASIENTOS)
	{
		db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.y][reserva.asiento.x] = reserva;
		db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.y][reserva.asiento.x].estado = ESTADOASIENTORESERVADO;
		return  true;
	}
	return false;	
}

bool dbComprarAsiento(struct db &db, const struct reserva &reserva)
{
	if(reserva.idSala < cantidadSalas && reserva.asiento.x < CANTIDADMAXASIENTOS, reserva.asiento.y < CANTIDADMAXASIENTOS)
	{
		db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.y][reserva.asiento.x] = reserva;
		db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.y][reserva.asiento.x].estado = ESTADOASIENTOCOMPRADO;
	}
	return false;	
}

bool dbLiberarAsiento(struct db &db, const struct reserva &reserva)
{
	if(reserva.idSala < cantidadSalas && reserva.asiento.x < CANTIDADMAXASIENTOS, reserva.asiento.y < CANTIDADMAXASIENTOS)
	{
		db.estadoAsientosEnSalas[reserva.idSala][reserva.asiento.y][reserva.asiento.x].estado = ESTADOASIENTOLIBRE;
	}
	return false;
}

struct salas dbObtenerSalas(const struct db &db) const
{
	struct salas salas;
	salas.totalsalas = db.cantidadSalas;

	for(unsigned int i = 0; i < db.cantidadSalas; i++)
		salas.numero[i] = i;

	return salas;
}

const asientos &dbObtenerAsientosSala(const struct db &db, unsigned int idSala) const
{
	return db.estadoAsientosEnSalas[idSala]; 

}
