#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


struct mensaje {
   long tipo;
   char mensa;
} mensaje;


int main() {
   int msgid;
   struct mensaje m;
   msgid = msgget(0xA,0);
   printf("msg id %d\n",msgid);
   m.tipo = 1L;
   m.mensa = 'x';
   while(msgsnd(msgid,&m,1,0) != -1) ;
   return 0;
}

