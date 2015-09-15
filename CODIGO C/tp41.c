/*
   tp41.c programa que captura las dos señales del usuario
   para compilar:
            gcc -Wall -o tp41 tp41.c 
            
   Guillermo Cherencio
   SO
   UTN FRD
*/
#include <stdio.h>
#include <stdlib.h>
#include	<signal.h>
#include	<unistd.h>

void sig_usr(int signo);

volatile int salir=0;

int main(void) {
	if (signal(SIGUSR1, sig_usr) == SIG_ERR) fprintf(stderr,"main():imposible capturar SIGUSR1\n");
	if (signal(SIGUSR2, sig_usr) == SIG_ERR) fprintf(stderr,"main():imposible capturar SIGUSR2\n");
	//if (signal(SIGINT, sig_usr) == SIG_ERR) fprintf(stderr,"main():imposible capturar SIGINT\n");

	while (!salir) {
      printf("main():ingreso en pause() salir=%d\n",salir);
		pause();
   }
   printf("main():sali normalmente\n");
}

void sig_usr(int signo)	{	/* signo=nro de señal */
	if (signo == SIGUSR1) { printf("sig_usr():recibi SIGUSR1 (%d)\n",signo); }
	else if (signo == SIGUSR2)  { printf("sig_usr():recibi SIGUSR2 (%d)\n",signo);salir=1;  }
	else if (signo == SIGINT)  { printf("sig_usr():recibi SIGINT (%d)\n",signo);}
	else { printf("sig_usr():recibi señal %d\n", signo); }
	return;
}

