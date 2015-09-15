#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
int main(){
	pid_t pid,pid1;
	int a=4;
	pid=fork();//hijo 1
	if (pid>0){
		printf("Soy el proc padre , id: %d, idPadre:%d\n",getpid(),getppid());
		printf("a= %d",a);
		pid1=fork();//hijo 2
		
		}else{
			printf("Soy el proc hijo1 , id: %d, idPadre:%d\n",getpid(),getppid());
			a=a*a;
			printf("a= %d",a);
			pid=fork();//nieto1
			if(pid==0){
				printf("Soy el proc nieto 1 , id: %d, idPadre:%d\n",getpid(),getppid());
				a++;
				printf("a= %d",a);
				exit(0);
				
				}else{
					wait(NULL);
					}
			wait(NULL);
			}
	if(pid1==0){
		printf("Soy el proc hijo2 , id: %d, idPadre:%d\n",getpid(),getppid());
		a=a*5;
		printf("a= %d",a);
		pid1=fork();//nieto 2
		if(pid1==0){
			printf("Soy el proc nieto 2, id: %d, idPadre:%d\n",getpid(),getppid());
			a=a+15;
			printf("a= %d",a);
			exit(0);
			}else{
				wait(NULL);
				}
		
		}else{
			wait(NULL);
			};
	
	exit(0);
	}
