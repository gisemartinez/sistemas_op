/*
   tp51.c Pipes. Programa que toma por teclado el numero de sucursal y simula la cantidad de venta de la misma   
   para compilar:
            gcc -Wall -o tp51 tp51.c 
            
   Guillermo Cherencio
   SO
   UTN FRD
*/
#include <stdio.h>
#include <stdlib.h>
#include	<unistd.h>
#include <string.h>
#include <time.h>

#define MAX_BUFFER 255

int main(int argc,char *argv[]) {
   // obtengo nro de sucursal de stdin
   char buffer[MAX_BUFFER];
   read(STDIN_FILENO,buffer,MAX_BUFFER);
   int suc = atoi(buffer);
   srandom(time(0)+suc);
   if ( suc >= 0 && suc < 10 ) { // sucursal valida
      double ventas=((double) random())/10000000.0;
      snprintf(buffer,MAX_BUFFER,"suc[%d]=%lf\n",suc,ventas);
   } else { // sucursal invalida
      snprintf(buffer,MAX_BUFFER,"suc[%d]=%lf\n",suc,-1.0);
   }
   // emito salida por stdout
   write(STDOUT_FILENO,buffer,strlen(buffer));
   return 0;
}

