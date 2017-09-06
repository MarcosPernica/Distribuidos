#ifndef BASEDEDATOS_H_
#define BASEDEDATOS_H_
#include "../entidades.h"
#include <vector>
#include <map>

#define CANTIDADPREFERIDASALAS 6

struct db
{	//Arreglo tridimensional (Numero de sala, Numero de fila, Numero ded columna)
	struct reserva estadoAsientosEnSalas[CANTIDADMAXSALAS][CANTIDADMAXASIENTOS][CANTIDADMAXASIENTOS];
	//Almacena las salas y que usuarios estan en cada una.
	std::multimap<unsigned int, unsigned int> usuariosActivos;
	unsigned int cantidadSalas;
}

//Inicializa la base de datos
bool dbCrear(struct db &db, unsigned int cantidadSalas = CANTIDADPREFERIDASALAS);

bool dbEntrarSala(struct db &db, struct elegirSala);

bool dbSalirSala(struct db &db, struct elegirSala);

//Las siguientes primitivas operan sobre la sala especificada en 'reserva'
bool dbReservarAsiento(struct db &db, const struct reserva &reserva);

bool dbComprarAsiento(struct db &db, const struct reserva &reserva);

bool dbLiberarAsiento(struct db &db, const struct reserva &reserva);

struct salas dbObtenerSalas(const struct db &db) const;

const asientos &dbObtenerAsientosSala(const struct db &db, unsigned int idSala) const;




#endif 
