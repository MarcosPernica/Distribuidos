CC = g++
DEBUG = -g -std=c++11

COMMON_DEPENDENCIES = common.h mensajes.h entidades.h ipc/cola.h ipc/semaforo.h ipc/memoriacompartida.h ipc/senal.h baseDeDatos/baseDeDatos.h
COMMON_SRCS= cola.cpp semaforo.cpp memoriacompartida.cpp senal.cpp entidades.cpp

CINE_SRCS= $(COMMON_SRCS) baseDeDatos.cpp cinehijo.cpp  cine.cpp administrador.cpp
CLIENT_SRCS= $(COMMON_SRCS) clienteAsinc.cpp cliente.cpp

CINE_OBJS=$(subst .cpp,.o,$(CINE_SRCS))
CLIENT_OBJS= $(subst .cpp,.o,$(CLIENT_SRCS))

all: client cine

client: $(CLIENT_OBJS)
	$(CC) $(DEBUG) -o client $(CLIENT_OBJS)
	
cine: $(CINE_OBJS)
	$(CC) $(DEBUG) -o cine $(CINE_OBJS)

#--CINE---
cine.o: cine.cpp cinehijo.h $(COMMON_DEPENDENCIES) baseDeDatos/baseDeDatos.h administrador.h
	$(CC) $(DEBUG) -c cine.cpp

cinehijo.o: cinehijo.cpp cinehijo.h $(COMMON_DEPENDENCIES)
	$(CC) $(DEBUG) -c cinehijo.cpp

baseDeDatos.o:  baseDeDatos/baseDeDatos.cpp entidades.h baseDeDatos/baseDeDatos.h 
	$(CC) $(DEBUG) -c baseDeDatos/baseDeDatos.cpp

administrador.o: administrador.cpp administrador.h $(COMMON_DEPENDENCIES) 
	$(CC) $(DEBUG) -c administrador.cpp
#-----

#--CLIENTE--
cliente.o: cliente.cpp clienteAsinc.h $(COMMON_DEPENDENCIES)
	$(CC) $(DEBUG) -c cliente.cpp

clienteAsinc.o: clienteAsinc.cpp clienteAsinc.h $(COMMON_DEPENDENCIES)
	$(CC) $(DEBUG) -c clienteAsinc.cpp
#------

#---COMMON---
entidades.o: entidades.cpp entidades.h
	$(CC) $(DEBUG) -c entidades.cpp

cola.o: ipc/cola.cpp ipc/cola.h
	$(CC) $(DEBUG) -c ipc/cola.cpp

semaforo.o: ipc/semaforo.cpp ipc/semaforo.h
	$(CC) $(DEBUG) -c ipc/semaforo.cpp

memoriacompartida.o: ipc/memoriacompartida.cpp ipc/memoriacompartida.h
	$(CC) $(DEBUG) -c ipc/memoriacompartida.cpp

basededatos.o: baseDeDatos/baseDeDatos.cpp baseDeDatos/baseDeDatos.h
	$(CC) $(DEBUG) -c baseDeDatos/baseDeDatos.cpp
	
senal.o: ipc/senal.cpp ipc/senal.h
	$(CC) $(DEBUG) -c ipc/senal.cpp

#------
		
cleanCliente:
	if [ -d ./build/client/ ]; then rm -R ./build/client; fi
	if [ -f client ]; then rm client; fi

cleanCine:
	if [ -d ./build/cine/ ]; then rm -R ./build/cine/; fi
	if [ -f cine ]; then rm cine; fi
	
clean: cleanCliente cleanCine
	rm -f *.o

