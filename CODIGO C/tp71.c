/*
   tp71.c IPC. Cola de mensajes. Message Queue. Programa servidor que lee de una cola
   de mensajes las peticiones de los clientes.  Las peticiones son mensajes de tipo 1.
   El mensaje contiene el id de proceso del cliente y el nro de sucursal de la cual desea saber
   el total de ventas. Formato del mensaje: <id proceso/nro.sucursal>
   El servidor responde a los clientes con un mensaje de tipo=id de proceso del cliente y el 
   formato del mensaje de respuesta es: <nro.sucursal/total ventas>
         
   para compilar:
            gcc -Wall -o tp71 tp71.c myipc.c
            
   Guillermo Cherencio
   SO
   UTN FRD
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h>
#include <ctype.h> 
#include	<fcntl.h>
#include	<unistd.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <signal.h> 

#include "myipc.h"

#define SUC_MSG_LEN 20 

struct sucmsg {
   long mtype; 
   char mtext[SUC_MSG_LEN]; 
};

// datos globales
int salir=0;
double ventas[10];

// prototipo de funciones
static void sig_salir(int);

int main(int argc, char *argv[])  {
   int ret=0;
   key_t key; 
   int idcola; 
   struct sucmsg qbuf; 

   printf("main():inicio servidor Cola de Mensajes!\n");
   printf("main():para salir envie señal SIGUSR2 a proceso %d\n",(int) getpid());
   printf("main():inicializo total de ventas de sucursales\n");
   // inicializo totales de ventas
   init(&ventas[0]);
   
   // capturar señal SIGUSR2
   if (signal(SIGUSR2,sig_salir) == SIG_ERR) fprintf(stderr,"main():Error capturando señal SIGUSR2\n");
   
   /* Create unique key via call to ftok() */ 
   printf("main():solicito clave ipc\n");
   key = obtener_clave(IPC_PATH, IPC_KEY); 

   /* Open the queue -create if necessary */ 
   printf("main():creo cola de mensajes con clave ipc\n");
   if ((idcola = crear_cola(key)) != -1) { 
      printf("main():quedo a la espera de mensajes en cola [%d]...\n",idcola);
      while(!salir) {
         // leo mensajes de tipo 1
         if ( recibir_msg(idcola,&qbuf,SUC_MSG_LEN,1L) != -1) {
            printf("main():recibi [%s] como peticion\n",qbuf.mtext);
         }
      }
      printf("main():fin recepcion de mensajes, borra cola [%d]\n",idcola);
      borrar_cola(idcola);
   } else {
      fprintf(stderr,"main():Error creando cola de mensajes\n"); 
      ret=1;
   } 
   printf("main():fin servidor  Cola de Mensajes! retorno=%d\n",ret);
   return ret;
} 

static void sig_salir(int signo) {
   salir=1;
   printf("main():recibi señal de salida\n");
}

