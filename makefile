CC = g++
DEBUG = -g
COMMON_DEPENDENCIES = common.h mensajes.h entidades.h mensajes.h ipc/cola.h ipc/semaforo.h ipc/memoriacompartida.h
SRCS= cola.cpp semaforo.cpp memoriacompartida.cpp baseDeDatos.cpp cinehijo.cpp  cine.cpp administrador.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

cine: $(OBJS)
	$(CC) $(DEBUG) -o cine $(OBJS)
	if [ ! -d ./build/ ]; then mkdir ./build/; fi
	mv *.o ./build/

cine.o: cine.cpp cinehijo.h $(COMMON_DEPENDENCIES) baseDeDatos/baseDeDatos.h
	$(CC) $(DEBUG) -c cine.cpp

cinehijo.o: cinehijo.cpp cinehijo.h $(COMMON_DEPENDENCIES)
	$(CC) $(DEBUG) -c cinehijo.cpp

baseDeDatos.o:  baseDeDatos/baseDeDatos.cpp entidades.h baseDeDatos/baseDeDatos.h 
	$(CC) $(DEBUG) -c baseDeDatos/baseDeDatos.cpp

administrador.o: administrador.cpp $(COMMON_DEPENDENCIES)
	$(CC) $(DEBUG) -c administrador.cpp

cola.o: ipc/cola.cpp ipc/cola.h
	$(CC) $(DEBUG) -c ipc/cola.cpp

semaforo.o: ipc/semaforo.cpp ipc/semaforo.h
	$(CC) $(DEBUG) -c ipc/semaforo.cpp

memoriacompartida.o: ipc/memoriacompartida.cpp ipc/memoriacompartida.h
	$(CC) $(DEBUG) -c ipc/memoriacompartida.cpp

clean:
	rm -f $(OBJS)

