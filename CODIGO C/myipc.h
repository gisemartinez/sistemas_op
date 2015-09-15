/*
   myipc.h IPC. Cola de mensajes, memoria compartida,  semaforos
   
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
#include <sys/shm.h> 
#include <sys/sem.h> 
#include <signal.h> 

#define IPC_PATH       "."    // path general para obtener claves ipc
#define IPC_KEY         'm'   // letra para cola de mensajes
#define IPC_SKEYs      's'    // letra para semaforo s
#define IPC_SKEYn      'n'    // letra para semaforo s
#define IPC_SKEYe      'e'    // letra para semaforo s
#define SEM_s            0    // id (dentro del vector de semaforos) para el semaforo s
#define SEM_n            1    // id (dentro del vector de semaforos) para el semaforo s
#define SEM_e            2    // id (dentro del vector de semaforos) para el semaforo s
#define IPC_MKEY      'h'    // letra para memoria  compartida


struct sucvta {
   int sucursal;
   char fecha[11]; // almacena tambien el \0 final
   int factura;
   double monto;
};

// definicion de estas macros no encontrada
#define SEM_R 0
#define SEM_A 0

// requerido para usar semctl()
union semun {
     int val;
     struct semid_ds *buf;
     unsigned short int *array;
     //struct seminfo *__buf;		<= buffer for IPC_INFO
};

// funciones en general para ipc
key_t obtener_clave(char *path,char car);
void obtener_claves(char *path[],char car[],key_t arrkey[],int nkeys);

// funciones para el manejo de colas
int borrar_cola(int qid); 
int crear_cola(key_t key);
int enviar_msg(int qid, void *qbuf, int msglen, long type); 
int obtener_cola(key_t key);
int recibir_msg(int qid, void *qbuf,int msglen, long type); 

// funciones para el manejo de memoria compartida
int borrar_memoria(int shmid);
void *conectar_memoria(int shmid,int modo);
int crear_memoria(key_t key,int size,int modo);
int desconectar_memoria(void *buffer);
/* Obtiene memoria compartida previamente creada */ 
int obtener_memoria(key_t key,int modo);


// funciones para el manejo de semaforos
int borrar_semaforo(int semid);
int borrar_semaforos(int semid,int nsem);
int semSignal(int semid);
int semWait(int semid);
int semSignalWaitTo(int semid,int nro);
int semSignalTo(int semid,int nro);
int semSignalWaitsTo(int semid,struct sembuf sb[],int nsem);
int semSignalsTo(int semid,struct sembuf sb[],int nsem) ;
int semSignalWaitsToNum(int semid,int nro,int nsem);
int semSignalsToNum(int semid,int nro,int nsem);
int crear_semaforo(key_t key,int modo);
int crear_semaforos(key_t key,int nsem,int modo) ;
int obtener_semaforo(key_t key,int modo);


// otras funciones
void init(double *ventas);
void mostrar_ventas(double *ventas);
void mostrar_sucventas(FILE *,struct sucvta *);
void generar_venta(struct sucvta *);

