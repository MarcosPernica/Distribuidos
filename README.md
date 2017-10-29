# Distribuidos
## Para correr
  - `make all` para compilar los ejectuables
  - `./init` para inicializar las colas
  - `./cine` para correr el cine
  - `./mom` para correr el mom
  - `./client` para correl el client
  - Se pueden frenar tanto el MOM como el cine con `Ctrl-C`
  - Si ocurrio un problema indeseado correr `./destroy` para eliminar las colas  
  
## TP1

### Enunciado
 Armar un sistema de reservas de asientos de cine mediante el cual un usuario:
- Ingrese al sistema
- Vea las salas disponibles
- Elija una sala
- Elija uno o mas asientos para reservar
- Finalice la compra
- Reciba actualizaciones de las reservas y compras de los otros usuarios

### Resolucion
En cuanto a la estrucutra del sistema se eligio implementarlo de la siguiente forma:
- Haber un proceso cine que recibe los eventos de login
- Un proceso hijo del cine por cada cliente que esta reservando asientos
- El cliente debe tener otro proces hijo que reciba los eventos asincronicos
- Un proceso administrador del cine debe notificar a los clientes los cambios

Toda la comunicacion entre procesos es por colas, memoria compartida y/o semaforos.

## TP2

### Enunciado
  Respetando la interaccion del usuario anterior se debe armar un MOM para 
  separar la interfaz visible cliente, escondiendo asi, la comunicacion entre procesos
  para el front end.
  Se debe respetar que:
  - El cliente se debe manejar mediante un Id al registrarse en el MOM
  - La api del cliente se comunica por colas al proceso del MOM
  - El MOM se comunica con el cine
  
### Resolucion
  Se separo el proceso cliente en dos. Este accede a los datos del sistema mediante una api
  que le da los siguientes metodos:
  - APIInit()
  - APILogin(int fd, struct login &login)
  - APIPedirSalas(int fd, struct salas &salas)
  - APIPedirAsientosSala(int fd, int idSala, struct sala &estadoAsientos)
  - APIObtenerActualizacionAsientos(int fd, struct sala &estadoAsientos)
  - APIElegirAsiento(int fd, struct asiento &asiento)
  - APIFinalizarCompra(int fd)
  - APIDeinit(int fd)

  Estos son enivados al MOM que se encarga de crear los procesos que atienden las actualizaciones
  del cine y envia los mensajes a las colas de login y cine.

## TP3

### Enunciado
  Manteniendo los enunciados anteriores posibilitar el acceso al cine desde maquinas distintas
  posibilitando que varios clientes de distintas maquinas se conecten al mismo cine por la red.
	
### Resolucion
  Se crearon 4 procesos para manejar las conexiones entre el cine y los clientes. En la maquina cliente habra
  un socket cliente que conectara al cine y enviara todos los mensajes que reciba de la cola login y de cine. 
  La administracion de los mensajes sera secuencial, si varios clientes interactuan a la vez desde una sola maquina
  cada mensaje enviado se enviara y respondera de forma secuencial. Tambien habra un sokcet servidor para que el cine 
  se conecte a este para enviar las actualizaciones.
  En paralelo el cine tendra un socket servidor que forkea cada conexion recibida para manejar los mensajes recibidos y 
  meterlos  en las colas de login o cine segun corresponda. Tambien tendra un proceso que maneja las conexiones a los
  clientes para enviar las actualizaciones de forma secuencial, es decir, si recibe las actualizcaciones para los clientes 
  3,4,5 enviara un mensaje tras otro.

  Se agrego un proceso mas para administrar los id de los clientes que al recibir una conexion envia un id nuevo y se desconecta.

  Para ver la comunicacion entre procesos y memoria compartida ver el grafico "Procesos de Trabajo" en la carpeta documentacion.
