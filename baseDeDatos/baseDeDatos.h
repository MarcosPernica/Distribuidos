#ifndef BASEDEDATOS_H_
#define BASEDEDATOS_H_
#include "../entidades.h"
#include "../mensajes.h"
#include <vector>
#include <map>

#define CANTIDADPREFERIDASALAS 6

struct db
{	//Arreglo tridimensional (Numero de sala, Numero de fila, Numero de columna)
	struct reserva estadoAsientosEnSalas[CANTIDADMAXSALAS][CANTIDADMAXASIENTOS][CANTIDADMAXASIENTOS];
	//Almacena las salas y que usuarios estan en cada una.
	std::multimap<unsigned int, unsigned int> usuariosPorSala;
	std::map<unsigned int, unsigned int> salaPorUsuario;
	unsigned int cantidadSalas;
};

//Inicializa la base de datos
bool dbCrear(struct db &db, unsigned int cantidadSalas = CANTIDADPREFERIDASALAS);

//Manejo de usuarios en salas
bool dbEntrarSala(struct db &db, struct elegirSala consulta);

//Solo requiere el userid
bool dbSalirSala(struct db &db, struct elegirSala consulta);

std::vector<unsigned int> dbObtenerUsuariosSala(struct db &db, struct elegirSala);

//Las siguientes primitivas operan sobre la sala especificada en 'reserva'
bool dbReservarAsiento(struct db &db, const struct reserva &reserva);

bool dbComprarAsiento(struct db &db, const struct reserva &reserva);

bool dbLiberarAsiento(struct db &db, const struct reserva &reserva);

struct salas dbObtenerSalas(const struct db &db);

const asientos &dbObtenerAsientosSala(const struct db &db, unsigned int idSala);

//Obtiene los usuarios en una sala

std::vector<unsigned int> dbObtenerUsuariosSala(const struct db &db, unsigned int idSala);  

#endif 
