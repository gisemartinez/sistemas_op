/*
 Programa Emulador de un Planificador de Procesos de dos estados
 TP.I
 UTN FRD 
 SO 2009
 Guillermo Cherencio
*/

#include <stdio.h>
#include <string.h>

#define RUNNING     1
#define NOT_RUNNING 0
#define MAX_PROC    5
#define START_PROC  0
#define MAX_INSTR   4
#define PROGRAM_LEN 64

// intrucciones posibles
#define ICALC       0
#define IPUTMEM     1
#define IGETMEM     2
#define ILOGIC      3
#define IIO         4
#define IEND        99

// programas
#define PROG_1      ICALC, ICALC, ILOGIC, IPUTMEM, IIO, ICALC, ICALC, IEND
#define PROG_2      ICALC, ILOGIC, ILOGIC, ICALC, IGETMEM, ICALC, ICALC, IEND
#define PROG_3      ICALC, ICALC, ILOGIC, IPUTMEM, ICALC, ICALC, ICALC, IEND
#define PROG_4      ICALC, ILOGIC, ILOGIC, IPUTMEM, IGETMEM, ICALC, ICALC, IEND
#define PROG_5      ICALC, ICALC, ILOGIC, IPUTMEM, IGETMEM, IPUTMEM, ILOGIC, ILOGIC, IEND

typedef struct {
   int id;
   int state;
   int pc;
   int code[PROGRAM_LEN];   
} pcb;

pcb cola[MAX_PROC];
int nproc=START_PROC;
int progs[MAX_PROC][PROGRAM_LEN] = { {PROG_1}, {PROG_2},{PROG_3},{PROG_4},{PROG_5}, };
int notFinished=0;

void pcbinit();
void pcbprint();
void run();
void pcbrun(int);

int main() {
   pcbinit();
   pcbprint();
   run();   
   pcbprint();
   return 0;
}

void pcbinit() {
   int n;
   for(n=0;n<MAX_PROC;n++) {
      cola[n].id=n+100;
      cola[n].state=NOT_RUNNING;
      cola[n].pc=0;
      // cargo instrucciones de procesos
      memcpy(&cola[n].code,&progs[n][0],sizeof(int)*PROGRAM_LEN);
   }
}

void pcbprint() {
   int n;
   for(n=0;n<MAX_PROC;n++) {
      char *status = (cola[n].state == RUNNING) ? "RUNNING" : "NOT RUNNING";
      printf("proc[%d] status=%s pc=%d instr=%d\n",cola[n].id,status,cola[n].pc,cola[n].code[cola[n].pc]);
   }
}

void pcbcheckend() {
   int n;
   notFinished=MAX_PROC;
   for(n=0;n<MAX_PROC;n++) {
      if (cola[n].code[cola[n].pc] == IEND) notFinished--;
   }
}

void run() {
   pcbcheckend();
   while( notFinished ) {  // hay al menos un proceso no finalizado
      //printf("notFinished=%d\n",notFinished);
      printf("Running Dispatcher\n");
      if ( cola[nproc].pc < PROGRAM_LEN ) {
         if ( cola[nproc].code[cola[nproc].pc] != IEND ) { // no es el fin del programa
            printf("running proc [%d] pc=%d\n",cola[nproc].id,cola[nproc].pc);
            cola[nproc].state = RUNNING;
            // ejecuto instruccion
            int ni=0;
            while (ni < MAX_INSTR && cola[nproc].code[cola[nproc].pc] != IEND) {
               if (cola[nproc].code[cola[nproc].pc] == IIO) ni=MAX_INSTR;
               pcbrun(nproc);
               ni++;
            } 
            cola[nproc].state = NOT_RUNNING;
            if (cola[nproc].code[cola[nproc].pc] == IEND) 
               printf("fin running proc [%d] pc=%d\n",cola[nproc].id,cola[nproc].pc);
         } // fin de programa
      } // fin de programa
      nproc++;
      if (nproc >= MAX_PROC) nproc=0;
      pcbcheckend();
      char c =  getchar();
   }
   printf("No hay mas procesos para ejecutar!\n");
}

void pcbrun(int np) {
            switch(cola[np].code[cola[np].pc]) {
               case ICALC:
                  printf("\tproc [%d] instruccion ICALC(%d)\n",cola[np].id,ICALC);
                  break;
               case IPUTMEM:
                  printf("\tproc [%d] instruccion IPUTMEM(%d)\n",cola[np].id,IPUTMEM);
                  break;
               case IGETMEM:
                  printf("\tproc [%d] instruccion IGETMEM(%d)\n",cola[np].id,IGETMEM);
                  break;
               case ILOGIC:
                  printf("\tproc [%d] instruccion ILOGIC(%d)\n",cola[np].id,ILOGIC);
                  break;
               case IIO:
                  printf("\tproc [%d] instruccion IIO(%d)\n",cola[np].id,IIO);
                  break;
            }
            cola[np].pc++;
}
