#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct mensaje {
   long tipo;
   char mensa;
} mensaje;


int main() {
   int msgid;
   struct mensaje m;
   memset(&m,0,sizeof(mensaje));    
   msgid = msgget(0xA,0);
   printf("msg id %d\n",msgid);
   while(1) {
      msgrcv(msgid,&m,1,1L,0);
      printf("%ld %c\n",m.tipo,m.mensa);
   }
   return 0;
}

