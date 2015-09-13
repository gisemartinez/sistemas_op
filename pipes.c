#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
void Padre(void);
void Hijo1(void);
void Hijo2(void);
int p1[2],p2[2] ;
char letra , letraant;
pid_t pid1 , pid2 ;
int main(void)
{

	pipe(p1);
 	//en C, usar el nombre de un array equivale a apuntar al primer miembro del mismo
	// en éste caso p1 = &p1[0]
	pipe(p2);
	//////////Luego de crear los pipes, creo los hijos
	// Padre e hijos comparten los file descriptors de los pipes
	pid1 = fork();
	if ( pid1 == 0 ){//hijo 1
		Hijo1();
	}else{
		pid2 = fork() ;
		if (pid2 == 0){//hijo 2
 			//Se cierra el extremo que no se usa
 			// Si no se cierran, los EOF no serán devueltos
			close(p1[1]); //el hijo2 no escribe en el pipe 1
			close(p2[0]); //el hijo2 no lee del pipe 2
			while(1){
				//dentro de Hijo2 se ejecuta el break
				Hijo2();
			}
		}else{
			//contrario al Hijo2
			close(p1[0]); //el padre no lee del pipe 1
			close(p2[1]); //el padre no escribe en el pipe 2
			write(p1[1],"A.",2);
			while(1){
				//dentro del Padre se ejecuta el break
				Padre();	
			}
		}
	}
	exit(0) ;
}
void Padre(void){
	while(read(p2[0],&letra,1)>0){
		printf("Padre dice: Lee de p2 %c\n",letra);
		if ( letra == 'Z'){
			printf("Padre dice: Lei Z Escribi Z \n");
			write(p1[1],&letra,1);
 			close(p2[0]);
			close(p1[1]);
			close(p1[0]);
			exit(0) ; //si el proceso termina no es necesario hacer los close anteriores, solo es a modo
			//didáctico
		}
 		if ( letra != '.' ){
			letraant = letra ;
			write(p1[1],&letra,1);
		}else{
			if ( letra == '.' ){
				letra = letraant + 1 ;
				write(p1[1],&letra,1);
				write(p1[1],".",1);
	 			if (letra == 'Z'){
					printf("Padre dice: Genere la Z Escribi la Z\n");
					close(p2[0]);
					close(p1[1]);
					close(p1[0]);
					kill(pid2,SIGINT);
					exit(0); //si el proceso termina no es necesario hacer los close anteriores, solo es a
				//modo didáctico
	 			}
				break;
			}			
		}
 	}//fin while
}

void Hijo2(void){
	while(read(p1[0],&letra,1)>0){
	 	printf("Hijo2 dice: Lee de p1 %c\n",letra);
	 	if ( letra == 'Z'){
			// printf("Hijo 2 dice: Lei Z Escribi Z \n");
			write(p2[1],&letra,1);
			close(p1[0]);
			close(p2[1]);
			close(p2[0]);
			exit(0); //si el proceso termina no es necesario hacer los close anteriores, solo es a modo 
			//didáctico
		}
		if ( letra != '.' ){
			letraant = letra ;
			write(p2[1],&letra,1);
		}else{
			if ( letra == '.' ){
				letra = letraant + 1 ;
				write(p2[1],&letra,1);
				write(p2[1],".",1);
				if (letra == 'Z'){
					printf("Hijo 2 dice: Genere la Z Escribi la Z\n");
					close(p1[0]);
					close(p2[1]);
					close(p2[0]);
					kill(getppid(),SIGINT);
					exit(0) ; //si el proceso termina no es necesario hacer los close anteriores, solo es a
	//modo didáctico
				}
				break;
			}
		}
	}
}
void Hijo1(void){
	close(p1[1]);
	close(p2[1]);
	printf("Hijo 1 dice: Espere tres segundos\n");
	sleep(3);
	printf("Hijo 1 dice: leo de p1\n");
	while(read(p1[0],&letra,1)>0){
		printf("%c " , letra);
		if ( letra == 'Z') break ;
	}
	printf("\n");
	printf("Hijo 1 dice: p1 Vacio\n");
	printf("Hijo 1 dice: leo de p2\n");
	while(read(p2[0],&letra,1)>0)
	{
		printf("%c " , letra);
		if ( letra == 'Z') break;
	}
	printf("\n");
	printf("Hijo 1 dice: p2 Vacio\n");
	exit(0);
}
