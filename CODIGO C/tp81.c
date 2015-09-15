/*
   tp81.c IPC. Memoria compartida. Proceso que guarda en memoria compartida el total de
   ventas de las sucursales. Queda bloqueado a la espera de la señal de salida.
         
   para compilar:
            gcc -Wall -o tp81 tp81.c myipc.c
            
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
#include <sys/shm.h> 
#include <signal.h> 

#include "myipc.h"

// datos globales
volatile int salir=0;
double ventas[10];

// prototipo de funciones
static void sig_salir(int);

int main(int argc, char *argv[])  {
   int ret=0;
   key_t key; 
   int idmem; 
   double *pventas;

   printf("main():inicio proceso productor!\n");
   printf("main():para salir envie señal SIGUSR2 a proceso %d\n",(int) getpid());
   
   // capturar señal SIGUSR2
   if (signal(SIGUSR2,sig_salir) == SIG_ERR) fprintf(stderr,"main():Error capturando señal SIGUSR2\n");
   
   /* Create unique key via call to ftok() */ 
   printf("main():solicito clave ipc\n");
   key = obtener_clave(IPC_PATH, IPC_MKEY); 

   /* Open the queue -create if necessary */ 
   int modo = SHM_R|SHM_W;
   int tamanio = sizeof(double)*10;
   printf("main():creo memoria compartida de %d bytes\n",tamanio);
   if ((idmem = crear_memoria(key,tamanio,modo)) != -1) { 
      printf("main():atachar memoria local a memoria compartida %d\n",idmem);
      if ((pventas = (double *) conectar_memoria(idmem,modo)) != (void *) -1) {
         printf("main():Inicializo memoria compartida a cero\n");
         memset(pventas,0,tamanio);
         printf("main():genero total de ventas de sucursales en memoria compartida\n");
         // inicializo totales de ventas
         init(pventas);
         mostrar_ventas(pventas);
         printf("main():Bloqueo proceso hasta recibir señal de salida\n");
         while(!salir) pause();
         printf("main():Desconecto memoria compartida\n");
         desconectar_memoria(pventas);
         printf("main():Destruyo memoria compartida\n");
         borrar_memoria(idmem);
      } else {
         printf("main():Error atachando memoria compartida\n");
         ret=2;
      }
   } else {
      fprintf(stderr,"main():Error creando memoria compartida\n"); 
      ret=1;
   } 
   printf("main():fin proceso productor! retorno=%d\n",ret);
   return ret;
} 

static void sig_salir(int signo) {
   salir=1;
   printf("main():recibi señal de salida\n");
}

