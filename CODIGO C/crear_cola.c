#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/*
struct mensaje {
   long tipo;
   char mensa;
} mensaje;
*/

int main() {
   int msgid;
   msgid = msgget(0xA,IPC_CREAT | 0700);
   printf("msg id %d\n",msgid);
   return 0;
}

