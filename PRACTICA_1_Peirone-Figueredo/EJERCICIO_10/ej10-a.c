#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <fcntl.h>

#define MAX_EVENTS 10
#define SHMEM_FILE "ej10_shm"

void quit(char *s) {
    perror(s);
    abort();

}

int *n;

int init_shm(size_t memsize) {
    int fd = shm_open(SHMEM_FILE, O_RDWR | O_CREAT, 0664);
    ftruncate(fd, memsize);

    return fd;
}

int fd_readline(int fd, char *buf) {
    int rc;
    int i = 0;

    /*
     *    * Leemos de a un caracter (no muy eficiente...) hasta
     *       * completar una línea.
     *          */
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

void handle_conn(int csock) {
    char buf[200];
    int rc;

    while (1) {
        /* Atendemos pedidos, uno por linea */
        rc = fd_readline(csock, buf);
        if (rc < 0)
            quit("read... raro");

        if (rc == 0) {
            /* linea vacia, se cerró la conexión */
            close(csock);
            return;

        }

        if (!strcmp(buf, "NUEVO")) {
            char reply[20];
            sprintf(reply, "%d\n", *n);
            (*n)++;
            write(csock, reply, strlen(reply));

        } else if (!strcmp(buf, "CHAU")) {
            fflush(stdout);
            close(csock);
            return;

        }

    }

}

void wait_for_clients(int lsock) {
    int conn_sock, shmfd;

    size_t memsize = sizeof(unsigned);

    shmfd = init_shm(memsize);
    n = mmap(NULL, memsize, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    *n = 0;

    while (1) {
        conn_sock = accept(lsock, NULL, NULL);
        if (fork() == 0) {
            handle_conn(conn_sock);
            munmap(n, memsize);
            close(shmfd);
            exit(EXIT_SUCCESS);
        }
        close(conn_sock);
    }
    munmap(n, memsize);
    close(shmfd);
    shm_unlink(SHMEM_FILE);
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

    /* Bindear al puerto 4040 TCP, en todas las direcciones
     * disponibles */
    rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
    if (rc < 0)
        quit("bind");

    /* Setear en modo escucha */
    rc = listen(lsock, 10);
    if (rc < 0)
        quit("listen");

    return lsock;

}

int main() {
    int lsock;
    lsock = mk_lsock();
    wait_for_clients_epoll(lsock);
    close(lsock);
}

