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

En cuanto a la estrucutra del sistema debe:
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
