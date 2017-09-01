CC = g++
DEBUG = -g
COMMON_DEPENDENCIES = common.h mensajes.h entidades.h mensajes.h ipc/cola.h ipc/semaforo.h ipc/memoriacompartida.h
SRCS= cola.cpp semaforo.cpp memoriacompartida.cpp cinehijo.cpp  cine.cpp administrador.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

cine: $(OBJS)
	$(CC) $(DEBUG) -o cine $(OBJS)
	mv *.o ./build/
#fatla abse de datos
cine.o: cine.cpp cinehijo.h $(COMMON_DEPENDENCIES) baseDeDatos/baseDeDatos.h
	$(CC) $(DEBUG) -c cine.cpp

cinehijo.o: cinehijo.cpp cinehijo.h $(COMMON_DEPENDENCIES)
	$(CC) $(DEBUG) -c cinehijo.cpp
 
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

