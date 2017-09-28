CC = g++
DEBUG = -g -std=c++11

COMMON_DEPENDENCIES = common.h mensajes.h entidades.h timeout.h ipc/cola.h ipc/semaforo.h ipc/memoriacompartida.h ipc/senal.h baseDeDatos/baseDeDatos.h  apiMensajes/apiMensajes.h
COMMON_SRCS= cola.cpp semaforo.cpp memoriacompartida.cpp senal.cpp entidades.cpp

CINE_SRCS= $(COMMON_SRCS) baseDeDatos.cpp cinehijo.cpp  cine.cpp administrador.cpp
CLIENT_SRCS= $(COMMON_SRCS) apiMensajes.cpp cliente.cpp
MOM_SRCS= $(COMMON_SRCS) clienteAsinc.cpp mom.cpp mom_run.cpp

CINE_OBJS=$(subst .cpp,.o,$(CINE_SRCS))
CLIENT_OBJS= $(subst .cpp,.o,$(CLIENT_SRCS))
MOM_OBJS= $(subst .cpp,.o,$(MOM_SRCS))

all: init destroy client cine mom
	if [ ! -d ./build/ ]; then mkdir ./build/; fi
	mv *.o ./build/
#--------
init: inicializador.o cola.o
	$(CC) $(DEBUG) -o init inicializador.o cola.o
destroy: destructor.o cola.o
	$(CC) $(DEBUG) -o destroy destructor.o cola.o

destructor.o: destructor/destructor.cpp ipc/cola.h common.h
	$(CC) $(DEBUG) -c destructor/destructor.cpp
	
inicializador.o: inicializador/inicializador.cpp ipc/cola.h common.h
	$(CC) $(DEBUG) -c inicializador/inicializador.cpp

#------

client: $(CLIENT_OBJS)
	$(CC) $(DEBUG) -o client $(CLIENT_OBJS)
	
cine: $(CINE_OBJS)
	$(CC) $(DEBUG) -o cine $(CINE_OBJS)

mom: $(MOM_OBJS)
	$(CC) $(DEBUG) -o mom $(MOM_OBJS)

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
cliente.o: cliente.cpp $(COMMON_DEPENDENCIES)
	$(CC) $(DEBUG) -c cliente.cpp

clienteAsinc.o: clienteAsinc.cpp clienteAsinc.h $(COMMON_DEPENDENCIES)
	$(CC) $(DEBUG) -c clienteAsinc.cpp
#------

#--MOM---
mom_run.o : mom_run.cpp $(COMMON_DEPENDENCIES) apiMensajes/mom.h
		$(CC) $(DEBUG) -c mom_run.cpp

mom.o: apiMensajes/mom.cpp apiMensajes/mom.h clienteAsinc.h $(COMMON_DEPENDENCIES)
	$(CC) $(DEBUG) -c apiMensajes/mom.cpp
#-----

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

apiMensajes.o: apiMensajes/apiMensajes.cpp apiMensajes/apiMensajes.h
	$(CC) $(DEBUG) -c apiMensajes/apiMensajes.cpp 

#------
		
cleanCliente:
	if [ -d ./build/client/ ]; then rm -R ./build/client; fi
	if [ -f client ]; then rm client; fi

cleanCine:
	if [ -d ./build/cine/ ]; then rm -R ./build/cine/; fi
	if [ -f cine ]; then rm cine; fi
	
clean: cleanCliente cleanCine
	rm -f *.o

