/*
   myfifo.h Pipes. FIFO. Archivo de cabecera a compartir entre servidor y clientes FIFO
   
   Guillermo Cherencio
   SO
   UTN FRD
*/

#define MAX_BUFFER 255         // deberia ser menor que PIPE_BUF
#define MY_FIFO "/tmp/myfifo"

// prototipo de funciones implementadas en myfifo.c
int crearfifo(char *pipe);
