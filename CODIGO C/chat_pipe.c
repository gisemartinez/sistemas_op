//      main.c
//      
//      Copyright 2011 grchere <grchere@debian2>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
	int p1[2],p2[2],status;
	pipe(p1);
	pipe(p2);
	pid_t pid = fork();
	if ( pid > 0 ) { // estoy en padre
		pid = fork();
		if ( pid == 0 ) { // estoy en hijo2
			pid = fork();
			if ( pid > 0 ) { // estoy en hijo2
				// escribir codigo de hijo2
				char entrada[512];
				close(p2[0]);
				close(p1[1]);
				close(p1[0]);
				do {
					printf("h2 envio:");
					memset(entrada,0,512);
					gets(entrada);
					write(p2[1],entrada,strlen(entrada)+1);
				} while(strcmp(entrada,"chau"));
				close(p2[1]);
				wait(&status);
			} else {
				// escribir codigo de nieto2
				char entrada[512];
				char byte;
				int lei,i;
				close(p1[1]);
				close(p2[1]);
				close(p2[0]);
				do {
					printf("n2 recibo:");
					memset(entrada,0,512);
					i=0;
					while ( (lei = read(p1[0],&byte,1)) && byte != '\0' ) {
						entrada[i++]=byte;
					}
					entrada[i] = '\0';
					printf("%s\n",entrada);
				} while(strcmp(entrada,"chau"));
				close(p1[0]);
			}
		} else { // estoy en padre
			// escribir codigo del padre
			close(p1[0]);close(p1[1]);
			close(p2[0]);close(p2[1]);
			wait(&status);
			wait(&status);
		}
	} else { // estoy en hijo1
		pid = fork();
		if ( pid > 0 ) { // estoy en hijo1
			// escribir codigo de hijo1
			char entrada[512];
			close(p1[0]);
			close(p2[1]);
			close(p2[0]);
			do {
				printf("h1 envio:");
				memset(entrada,0,512);
				gets(entrada);
				write(p1[1],entrada,strlen(entrada)+1);
			} while(strcmp(entrada,"chau"));
			close(p1[1]);
			wait(&status);
		} else { // estoy en nieto1
			// escribir codigo de nieto1
			char entrada[512];
			char byte;
			int lei,i;
			close(p2[1]);
			close(p1[1]);
			close(p1[0]);
			do {
				printf("n1 recibo:");
				memset(entrada,0,512);
				i=0;
				while ( (lei = read(p2[0],&byte,1)) && byte != '\0') {
					entrada[i++]=byte;
				}
				entrada[i] = '\0';
				printf("%s\n",entrada);
			} while(strcmp(entrada,"chau"));
			close(p2[0]);
		}
	}
	
	return 0;
}

