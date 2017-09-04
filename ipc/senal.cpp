#include "senal.h"

int registrarSenal(int signum, sighandler_t handling){
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handling;
	if (sigemptyset ( &sa.sa_mask ) == -1 ){
		return EMPTY_MASK_ERROR;
	}	// inicializa la mascara de seniales a bloquear durante la ejecucion del handler como vacio

	if ( sigaddset ( &sa.sa_mask,signum ) == -1 )//bloquea a si misma
	{
		return ADD_SIGNAL_ERROR;
	}

	if ( sigaction ( signum,&sa,0 ) ){ // cambiar accion de la seniales
		return ADD_ACTION_ERROR;
	}

	return 0;
}

int enviarSenal(int pid, int signum){
	return kill(pid,signum);
}
