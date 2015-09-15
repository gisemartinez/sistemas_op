/*
   tp61.c Pipes. FIFO. Programa servidor que lee aquello que los clientes
   graban en un FIFO abierto a tal efecto
   Plantea un ciclo infinito, queda bloqueado en read(), la salida es abrupta por envio de señal de terminacion
   no cierra FIFO, no elimina archivo FIFO
   
   para compilar:
            gcc -Wall -o tp61 tp61.c 
            
   Guillermo Cherencio
   SO
   UTN FRD
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include	<fcntl.h>
#include	<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <limits.h>

#include "myfifo.h"

int main(int argc,char *argv[]) {
      int ret=0;
      printf("main():inicio servidor FIFO!\n");
      
      int mf = mkfifo(MY_FIFO,O_CREAT);
      if ( mf == 0 || errno == EEXIST ) {
         chmod(MY_FIFO,0777);
         printf("main():Tamaño maximo de operaciones atomicas sobre PIPES: %d\n",PIPE_BUF);
         printf("main():Tamaño maximo de buffer de servidor: %d\n",MAX_BUFFER);

         int fdi = open(MY_FIFO,O_RDWR,0777);  // para no recibir EOF cuando se pasa de 1 cliente a 0 cliente

         char buffer[MAX_BUFFER+1];
         while(1) {
               //printf("main(): loop salir=%d entro en read()\n",salir);  // trace
               int nread = read(fdi,buffer,MAX_BUFFER);
               //printf("main():fin read()=%d errno=%d\n",nread,errno); // trace
               if ( nread != -1 ) {
                  buffer[nread]='\0';
                  printf("main():lei: [%s] de FIFO [%s]\n",buffer,MY_FIFO);
               } else {
                  printf("main():Error leyendo de FIFO [%s]\n",MY_FIFO);
               }
         } // while (1)
         
         close(fdi);
         
         // elimino FIFO
         remove(MY_FIFO);
         
      } else {
         fprintf(stderr,"main():Error creando FIFO %s\n",MY_FIFO);
         ret=1;
      }
      printf("main():fin servidor FIFO! retorno=%d\n",ret);
      return ret;
}


