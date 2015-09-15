//      main2.c
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
/*
 * 
 * 
En Debian y por consiguiente en Ubuntu, los paquetes necesarios para 
trabajar con GTK son:

libc6-dev
pkg-config
libglib2.0-dev
libgtk2.0-dev
* 
* Compilar:
* gcc -Wall -c "%f" `pkg-config --cflags gtk+-2.0 --libs` -lpthread
* 
* Construir:
* gcc -Wall -o "%e" "%f" `pkg-config --cflags gtk+-2.0 --libs` -lpthread
* 
* 
* Version del chat utilizando pipes con procesos emparentados, visualizando
* a los procesos en 4 ventanas GTK+ independientes, en donde los procesos lectores
* del pipe utilizan un thread para tal tarea e insertan lo leido n el texto de la 
* pantalla gtk
* 
* Atte. Guillermo Cherencio
* UTN FRD SO
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <gtk/gtk.h>

// funciones que crean las ventanas y componentes GTK+ para cada proceso
void win_h1(int argc, char **argv);
void win_h2(int argc, char **argv);
void win_n1(int argc, char **argv);
void win_n2(int argc, char **argv);

// funciones callback para eventos: windows open, windows close, boton envio click, en h1
void on_window_open_h1(GtkWidget *widget, gpointer data);
void on_window_destroy_h1(GtkWidget *widget, gpointer data);
void on_bsendh1_clicked(GtkWidget *button, GtkWidget *data);

// funciones callback para eventos: windows open, windows close, boton envio click, en h2
void on_window_open_h2(GtkWidget *widget, gpointer data);
void on_window_destroy_h2(GtkWidget *widget, gpointer data);
void on_bsendh2_clicked(GtkWidget *button, GtkWidget *data);

// funciones callback para eventos: windows close, en n1
void on_window_destroy_n1(GtkWidget *widget, gpointer data);

// funciones callback para eventos: windows close, en n2
void on_window_destroy_n2(GtkWidget *widget, gpointer data);

// funciones para gestionar la salida de h1 y h2
void win_h1_quit();
void win_h2_quit();

// funciones a ser lanzadas como threads para la lectura de pipes en n1 y n2
void *hago_n1( void *ptr );
void *hago_n2( void *ptr );

// variables globales a ser heredadas por h1, h2, n1, n2
int p1[2],p2[2],status;
int bIngreso = FALSE;

int main(int argc, char **argv)
{
	printf("gtkpipe() inicio proceso padre\n");
	pipe(p1);
	pipe(p2);
	pid_t pid = fork();
	if ( pid > 0 ) { // estoy en padre
		pid = fork();
		if ( pid == 0 ) { // estoy en hijo2
			pid = fork();
			if ( pid > 0 ) { // estoy en hijo2
			
				// escribir codigo de hijo2
				win_h2(argc,argv);
				
			} else {
				
				// escribir codigo de nieto2
				win_n2(argc,argv);
				
			}
		} else { // estoy en padre
		
			printf("gtkpipe() bloqueo proceso padre\n");					
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
			win_h1(argc,argv);
			
		} else { // estoy en nieto1
		
			// escribir codigo de nieto1
			win_n1(argc,argv);
			
		}
	}
  	printf("fin proceso pid=%d\n",(int) getpid());	
	return 0;
}

/*******************************************************************************
 * IMPLEMENTACION DE FUNCIONES QUE CREAN INTERFAZ GRAFICA DE USUARIO EN GTK+
 * ****************************************************************************/

void win_h1(int argc, char **argv) {
	pid_t p = getpid();
	char titulo[100];
	GtkWidget *ventana;
	GtkWidget *texth1;
	GtkWidget *bsendh1; 
	GtkWidget *wscrollh1;
	GtkWidget *vboxleft;	
	GtkTextBuffer *buffer_texth1;	 
	printf("inicio win_h1()\n");
	
	gtk_init (&argc, &argv);
	ventana = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	snprintf(titulo,100,"UTN FRD SO - Pipe Chat - H1 (pid=%d)",(int) p);
	gtk_window_set_title((GtkWindow *) ventana,titulo);
    gtk_window_set_default_size(GTK_WINDOW (ventana), 400, 300);
	gtk_window_move((GtkWindow *) ventana,50,50);
	// box
	vboxleft = gtk_vbox_new (TRUE, 2);    

	// text area h1
    texth1 = gtk_text_view_new();	
	buffer_texth1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(texth1));
    gtk_text_buffer_set_text(buffer_texth1, "", -1);    

	// vertical scrollbars
	wscrollh1 = gtk_scrolled_window_new(NULL,NULL);	
	gtk_container_set_border_width(GTK_CONTAINER(wscrollh1), 10);

	// boton de envio
    bsendh1 = gtk_button_new_with_label ("Enviar h1");
    // evento click sobre boton
    g_signal_connect (G_OBJECT(bsendh1), "clicked", 
                    G_CALLBACK(on_bsendh1_clicked),
                    texth1);	

	// agrego texto h1 a scrollbar
	gtk_scrolled_window_add_with_viewport (
                   GTK_SCROLLED_WINDOW(wscrollh1), texth1);	

	// agrego scrollbar a box
	gtk_box_pack_start (GTK_BOX(vboxleft),  wscrollh1, TRUE, TRUE, 0);

	// agrego boton a box
    gtk_box_pack_start (GTK_BOX (vboxleft), bsendh1, FALSE, FALSE, 0);

	// agrego box a ventana
    gtk_container_add (GTK_CONTAINER(ventana), vboxleft);

    // evento close sobre ventana principal
	g_signal_connect (G_OBJECT (ventana), "destroy", 
                    G_CALLBACK (on_window_destroy_h1),
                    NULL);    
	g_signal_connect (G_OBJECT (ventana), "window_state_event", 
                    G_CALLBACK (on_window_open_h1),
                    texth1);    
    // muestro todo                
    gtk_widget_show_all(ventana);
    
  	printf("inicio win_h1() gtk_main()\n");	
    // inicio loop
	gtk_main();

}

void win_h2(int argc, char **argv) {
	pid_t p = getpid();
	char titulo[100];
	GtkWidget *ventana;
	GtkWidget *texth1;
	GtkWidget *bsendh1; 
	GtkWidget *wscrollh1;
	GtkWidget *vboxleft;	
	GtkTextBuffer *buffer_texth1;	 
	printf("inicio win_h2()\n");
	
	gtk_init (&argc, &argv);
	ventana = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	snprintf(titulo,100,"UTN FRD SO - Pipe Chat - H2 (pid=%d)",(int) p);
	gtk_window_set_title((GtkWindow *) ventana,titulo);
    gtk_window_set_default_size(GTK_WINDOW (ventana), 400, 300);
	gtk_window_move((GtkWindow *) ventana,500,50);

	// box
	vboxleft = gtk_vbox_new (TRUE, 2);    

	// text area h1
    texth1 = gtk_text_view_new();	
	buffer_texth1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(texth1));
    gtk_text_buffer_set_text(buffer_texth1, "", -1);    

	// vertical scrollbars
	wscrollh1 = gtk_scrolled_window_new(NULL,NULL);	
	gtk_container_set_border_width(GTK_CONTAINER(wscrollh1), 10);

	// boton de envio
    bsendh1 = gtk_button_new_with_label ("Enviar h2");
    // evento click sobre boton
    g_signal_connect (G_OBJECT(bsendh1), "clicked", 
                    G_CALLBACK(on_bsendh2_clicked),
                    texth1);	

	// agrego texto h1 a scrollbar
	gtk_scrolled_window_add_with_viewport(
                   GTK_SCROLLED_WINDOW(wscrollh1), texth1);	

	// agrego scrollbar a box
	gtk_box_pack_start (GTK_BOX(vboxleft),  wscrollh1, TRUE, TRUE, 0);

	// agrego boton a box
    gtk_box_pack_start (GTK_BOX (vboxleft), bsendh1, FALSE, FALSE, 0);

	// agrego box a ventana
    gtk_container_add (GTK_CONTAINER(ventana), vboxleft);

    // evento close sobre ventana principal
	g_signal_connect (G_OBJECT (ventana), "destroy", 
                    G_CALLBACK (on_window_destroy_h2),
                    NULL);    
	g_signal_connect (G_OBJECT (ventana), "window_state_event", 
                    G_CALLBACK (on_window_open_h2),
                    texth1);    
    // muestro todo                
    gtk_widget_show_all(ventana);
    
  	printf("inicio win_h2() gtk_main()\n");	
	// inicio loop
	gtk_main();

}

void win_n1(int argc, char **argv) {
	pthread_t thr;
	pid_t p = getpid();
	char titulo[100];
	GtkWidget *ventana;
	GtkWidget *texth1;
	GtkWidget *wscrollh1;
	GtkWidget *vboxleft;
	GtkTextBuffer *buffer_texth1;
	printf("inicio win_n1()\n");
	gtk_init (&argc, &argv);
	ventana = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	snprintf(titulo,100,"UTN FRD SO - Pipe Chat - N1 (pid=%d)",(int) p);
	gtk_window_set_title((GtkWindow *) ventana, titulo);
    gtk_window_set_default_size(GTK_WINDOW (ventana), 400, 300);
	gtk_window_move((GtkWindow *) ventana,50,450);

	// box
	vboxleft = gtk_vbox_new (TRUE, 2);

	// text area h1
    texth1 = gtk_text_view_new();
	buffer_texth1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(texth1));
    gtk_text_buffer_set_text(buffer_texth1, "", -1);
	gtk_text_view_set_editable((GtkTextView *) texth1,FALSE);    

	// vertical scrollbars
	wscrollh1 = gtk_scrolled_window_new(NULL,NULL);
	gtk_container_set_border_width(GTK_CONTAINER(wscrollh1), 10);

	// agrego texto h1 a scrollbar
	gtk_scrolled_window_add_with_viewport(
                   GTK_SCROLLED_WINDOW(wscrollh1), texth1);

	// agrego scrollbar a box
	gtk_box_pack_start (GTK_BOX(vboxleft),  wscrollh1, TRUE, TRUE, 0);

	// agrego box a ventana
    gtk_container_add (GTK_CONTAINER(ventana), vboxleft);

    // evento close sobre ventana principal
	g_signal_connect (G_OBJECT (ventana), "destroy", 
                    G_CALLBACK (on_window_destroy_n1),
                    NULL);

	// lanzo thread para manejo de pipes
	 int rc = pthread_create(&thr, NULL, hago_n1,  (void *) texth1);
	 if (rc){ 
		printf("win_n1(): Error, pthread_create() retorna %d\n", rc);
	 }

    // muestro todo                
    gtk_widget_show_all(ventana);
	
	printf("inicio win_n1() gtk_main()\n");	
    // inicio loop
	gtk_main();

}

void win_n2(int argc, char **argv) {
	pthread_t thr;
	pid_t p = getpid();
	char titulo[100];
	GtkWidget *ventana;
	GtkWidget *texth1;
	GtkWidget *wscrollh1;
	GtkWidget *vboxleft;	
	GtkTextBuffer *buffer_texth1;	 
	
	printf("inicio win_n2()\n");
	gtk_init (&argc, &argv);
	ventana = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	snprintf(titulo,100,"UTN FRD SO - Pipe Chat - N2 (pid=%d)",(int) p);
	gtk_window_set_title((GtkWindow *) ventana,titulo);
    gtk_window_set_default_size(GTK_WINDOW (ventana), 400, 300);
	gtk_window_move((GtkWindow *) ventana,500,450);

	// box
	vboxleft = gtk_vbox_new (TRUE, 2);    

	// text area h1
    texth1 = gtk_text_view_new();	
	buffer_texth1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(texth1));
    gtk_text_buffer_set_text(buffer_texth1, "", -1);    
	gtk_text_view_set_editable((GtkTextView *) texth1,FALSE);    

	// vertical scrollbars
	wscrollh1 = gtk_scrolled_window_new(NULL,NULL);	
	gtk_container_set_border_width(GTK_CONTAINER(wscrollh1), 10);

	// agrego texto h1 a scrollbar
	gtk_scrolled_window_add_with_viewport(
                   GTK_SCROLLED_WINDOW(wscrollh1), texth1);	

	// agrego scrollbar a box
	gtk_box_pack_start (GTK_BOX(vboxleft),  wscrollh1, TRUE, TRUE, 0);

	// agrego box a ventana
    gtk_container_add (GTK_CONTAINER(ventana), vboxleft);

    // evento close sobre ventana principal
	g_signal_connect (G_OBJECT (ventana), "destroy", 
                    G_CALLBACK (on_window_destroy_n2),
                    NULL);    

	// lanzo thread para manejo de pipes
	 int rc = pthread_create(&thr, NULL, hago_n2,  (void *) texth1);
	 if (rc){ 
		printf("win_n1(): Error, pthread_create() retorna %d\n", rc);
	 }

    // muestro todo                
    gtk_widget_show_all(ventana);
    
	printf("inicio win_n2() gtk_main()\n");	
    // inicio loop
	gtk_main();

}

/*******************************************************************************
 * IMPLEMENTACION DE FUNCIONES DE CALLBACK PARA MANEJO DE EVENTOS
 * ****************************************************************************/

void on_window_destroy_h1(GtkWidget *widget, gpointer data) {
	char entrada[512];
	memset(entrada,0,512);
	strcpy(entrada,"chau");
	write(p1[1],entrada,strlen(entrada)+1);
	win_h1_quit();
}

void on_window_destroy_h2(GtkWidget *widget, gpointer data) {
	char entrada[512];
	memset(entrada,0,512);
	strcpy(entrada,"chau");
	write(p2[1],entrada,strlen(entrada)+1);
	win_h2_quit();
}

void on_window_destroy_n1(GtkWidget *widget, gpointer data) {
	bIngreso = FALSE;
}

void on_window_destroy_n2(GtkWidget *widget, gpointer data) {
	bIngreso = FALSE;
}

void on_window_open_h1(GtkWidget *widget, gpointer data) {
	close(p1[0]);
	close(p2[1]);
	close(p2[0]);
}

void on_window_open_h2(GtkWidget *widget, gpointer data) {
	close(p2[0]);
	close(p1[1]);
	close(p1[0]);
}

void on_bsendh1_clicked(GtkWidget *button, GtkWidget *data) {
	GtkTextBuffer *buffer_texth1;	 
	char entrada[512];
	char *p;
	
	buffer_texth1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data));
	GtkTextIter inicio, fin;
	gtk_text_buffer_get_start_iter(buffer_texth1, &inicio);
	gtk_text_buffer_get_end_iter(buffer_texth1, &fin);	

	memset(entrada,0,512);
	p = (char *) gtk_text_buffer_get_text(buffer_texth1,&inicio,&fin,FALSE);
	strcpy(entrada,p);
	write(p1[1],entrada,strlen(entrada)+1);
	if (!strcmp(entrada,"chau") ) { // tipeo chau!
		win_h1_quit();
	}
    gtk_text_buffer_set_text(buffer_texth1, "", -1);    
	
}

void on_bsendh2_clicked(GtkWidget *button, GtkWidget *data) {
	GtkTextBuffer *buffer_texth1;	 
	char entrada[512];
	char *p;
	
	buffer_texth1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(data));
	GtkTextIter inicio, fin;
	gtk_text_buffer_get_start_iter(buffer_texth1, &inicio);
	gtk_text_buffer_get_end_iter(buffer_texth1, &fin);	

	memset(entrada,0,512);
	p = (char *) gtk_text_buffer_get_text(buffer_texth1,&inicio,&fin,FALSE);
	strcpy(entrada,p);
	write(p2[1],entrada,strlen(entrada)+1);
	if (!strcmp(entrada,"chau") ) { // tipeo chau!
		win_h2_quit();
	}
    gtk_text_buffer_set_text(buffer_texth1, "", -1);    
}

/*******************************************************************************
 * IMPLEMENTACION DE FUNCIONES PARA LA SALIDA DE H1 Y H2
 * ****************************************************************************/

void win_h1_quit() {
	close(p1[1]);
	wait(&status);
	gtk_main_quit();
}

void win_h2_quit() {
	close(p2[1]);
	wait(&status);
	gtk_main_quit();
}

/*******************************************************************************
 * IMPLEMENTACION DE THREADS DE LECTURA DE PIPES EN N1 Y N2
 * ****************************************************************************/

void *hago_n1( void *ptr ) {
	// escribir codigo de nieto1
	bIngreso = TRUE;
	GtkWidget *texth1;
	GtkTextBuffer *buffer_texth1;	 
	GtkTextIter iter;
	char entrada[512];
	char byte;
	int lei,i;
	close(p2[1]);
	close(p1[1]);
	close(p1[0]);
	texth1 = (GtkWidget *) ptr;

	do {
		printf("hago_n1(): n1 leo de pipe2:\n");
		memset(entrada,0,512);
		i=0;
		while ( (lei = read(p2[0],&byte,1)) && byte != '\0') {
			entrada[i++]=byte;
		}
		entrada[i] = '\0';
		printf("hago_n1(): lei de pipe2 [%s]\n",entrada);
		buffer_texth1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(texth1));
		int nchars = (int) gtk_text_buffer_get_char_count(buffer_texth1);
		gtk_text_buffer_get_iter_at_offset (buffer_texth1, &iter, nchars);
		gtk_text_buffer_insert (buffer_texth1, &iter,entrada,-1);
	} while(bIngreso && strcmp(entrada,"chau"));
	close(p2[0]);
	gtk_main_quit();
	pthread_exit(NULL);	
}

void *hago_n2( void *ptr ) {
	bIngreso = TRUE;
	GtkWidget *texth1;
	GtkTextBuffer *buffer_texth1;	 
	GtkTextIter iter;
	char entrada[512];
	char byte;
	int lei,i;
	close(p1[1]);
	close(p2[1]);
	close(p2[0]);
	texth1 = (GtkWidget *) ptr;
	
	do {
		printf("hago_n2(): n2 leo de pipe1:\n");
		memset(entrada,0,512);
		i=0;
		while ( (lei = read(p1[0],&byte,1)) && byte != '\0' ) {
			entrada[i++]=byte;
		}
		entrada[i] = '\0';
		printf("hago_n2(): lei de pipe1 [%s]\n",entrada);
		buffer_texth1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(texth1));
		int nchars = (int) gtk_text_buffer_get_char_count(buffer_texth1);
		gtk_text_buffer_get_iter_at_offset (buffer_texth1, &iter, nchars);
		gtk_text_buffer_insert (buffer_texth1, &iter,entrada,-1);
	} while(bIngreso && strcmp(entrada,"chau"));
	close(p1[0]);
	gtk_main_quit();
	pthread_exit(NULL);	
}
