#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define MAXEVENT 64

/*
 * Para probar, usar netcat. Ej:
 *
 *      $ nc localhost 4040
 *      NUEVO
 *      0
 *      NUEVO
 *      1
 *      CHAU
 */

void quit(char *s) {
	perror(s);
	abort();
}

int U = 0;

int fd_readline(int fd, char *buf) {
	int rc;
	int i = 0;

	/*
	 * Leemos de a un caracter (no muy eficiente...) hasta
	 * completar una línea.
	 */
	while ((rc = read(fd, buf + i, 1)) > 0) {
		if (buf[i] == '\n')
			break;
		i++;
	}

	if (rc < 0)
		return rc;

	buf[i] = 0;
	return i;
}

void handle_conn(int csock, int epoll_fd_client, struct epoll_event client_event) {
	char buf[200];
	int rc;
    /* Atendemos pedidos, uno por linea */
    rc = fd_readline(csock, buf);
    if (rc < 0)
        quit("read... raro");

    if (rc == 0) {
        /* linea vacia, se cerró la conexión */
        printf("linea vacia... \n");
        close(csock);
        return;
    }
    if (!strcmp(buf, "NUEVO")) {
        char reply[20];
        sprintf(reply, "%d\n", U);
        reply[strlen(reply)] = '\0';
        U++;
        write(csock, reply, strlen(reply));
    } 
    else if (!strcmp(buf, "CHAU")) {
        printf("Conexion con cliente terminada.\n");
        epoll_ctl(epoll_fd_client, EPOLL_CTL_DEL, csock, &client_event);
        close(csock);
        return;
    }
}

void wait_for_clients(int lsock, int epoll_fd, struct epoll_event event, struct epoll_event* events) {
	int csock;
	int cant_events;
	printf("Esperando clientes...\n");
	while(1) {
		cant_events = epoll_wait(epoll_fd, events, MAXEVENT, -1);
        if(cant_events == -1) quit("epoll_wait");

        for(int i = 0; i < cant_events; i++) {
            if(events[i].data.fd == lsock) {

                /* Hubo un evento en el lsock, aceptamos el cliente que se quiere conectar */
                csock = accept(lsock, NULL, NULL);
                if (csock < 0)
                    quit("accept");

                /* Agregamos el file descriptor del cliente al epoll */
                event.data.fd = csock;
                event.events = EPOLLIN;
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, csock, &event) == -1)
                    quit("epoll_ctl");

                printf("Conexion con cliente establecida!\n");
            }
            else{
                /* Manejamos un evento de un cliente */
                handle_conn(events[i].data.fd, epoll_fd, event);
            }
        }
	}
}

/* Crea un socket de escucha en puerto 4040 TCP */
int mk_lsock() {
	struct sockaddr_in sa;
	int lsock;
	int rc;
	int yes = 1;

	/* Crear socket */
	lsock = socket(AF_INET, SOCK_STREAM, 0);
	if (lsock < 0)
		quit("socket");

	/* Setear opción reuseaddr... normalmente no es necesario */
	if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == 1)
		quit("setsockopt");

	sa.sin_family = AF_INET;
	sa.sin_port = htons(4040);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Bindear al puerto 4040 TCP, en todas las direcciones disponibles */
	rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
	if (rc < 0)
		quit("bind");

	/* Setear en modo escucha */
	printf("Listening...\n");
	rc = listen(lsock, 10);
	if (rc < 0)
		quit("listen");

	return lsock;
}

int main() {
	int lsock;
	lsock = mk_lsock();

	struct epoll_event event;
    struct epoll_event events[MAXEVENT];

	/* Creamos estructura epoll*/
	int epoll_fd = epoll_create1(0);
    if(epoll_fd == -1) quit("epoll_create1");

	/* Guardamos el lsock en el epoll creado*/
	event.data.fd = lsock;
	event.events = EPOLLIN;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, lsock, &event) == -1)
        quit("epoll_ctl");

	wait_for_clients(lsock, epoll_fd, event, events);
	close(lsock);
}