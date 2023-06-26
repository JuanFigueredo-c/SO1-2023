#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>

pthread_t dummy; // dummy thread for anonymous threads

void quit(char *s) {
  perror(s);
  abort();
}

int n = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int fd_readline(int fd, char *buf) {
  int rc;
  int i = 0;

  /*
   * Leemos de a un caracter (no muy eficiente...) hasta
   * completar una línea.
   * */
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

void cleanup() {
  puts("Cleaning up...");
  puts("Shutting down server");
  exit(EXIT_SUCCESS);
}

void handle_conn(int csock) {
  char buf[200];
  int rc;

  puts("Client connected");
  //write(csock, "Connected to server\n", 20);

  while (1) {
    /* Atendemos pedidos, uno por linea */
    rc = fd_readline(csock, buf);
    if (rc < 0)
      quit("read... raro");

    if (rc == 0)
      /* linea vacia, cerramos la conexion */
      break;

    if (!strcmp(buf, "NUEVO")) {
      char reply[20];
      pthread_mutex_lock(&mutex);
      sprintf(reply, "%d\n", n++);
      pthread_mutex_unlock(&mutex);
      write(csock, reply, strlen(reply));

    } else if (!strcmp(buf, "CHAU"))
      break;
  }
  puts("Client disconnected");
  write(csock, "Closing connection\n", 19);
  close(csock);
}

void wait_for_clients(int lsock) {
  int conn_sock;

  while (1) {
    conn_sock = accept(lsock, NULL, NULL);
    // The two step cast in the following line is to avoid compiler warnings
    // Specifically the -Wint-to-void-pointer-cast warning
    pthread_create(&dummy, NULL, (void* (*)(void*)) handle_conn, (void*) ((size_t) conn_sock));
  }

  // TODO Implement signal handler to execute clean up code
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
  sa.sin_port = htons(8000);
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

  puts("Starting server...");
  puts("Server listening on port 8000 ..");

  return lsock;

}

int main() {
  int lsock = mk_lsock();
  struct sigaction sa;
  
  sa.sa_handler = cleanup;
  sigaction(SIGINT, &sa, NULL);

  wait_for_clients(lsock);
}

