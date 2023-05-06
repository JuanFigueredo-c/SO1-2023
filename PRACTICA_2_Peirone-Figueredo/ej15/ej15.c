#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lamport.h"

#ifndef VPM
#define VPM 100000
#endif

int visitantes = 0;
int nthreads;
int cant_x_thread;

void* molinete(void* arg){
    int id = *(int*) arg;
    for (int n = 0; n < cant_x_thread; n++) {
        lock(id, nthreads);
        visitantes++;
        unlock(id);
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Numero incorrecto de argumentos\nUso: ./a.out <numero hilos> <cantidad por hilo>\n");
        exit(0);
    }
    nthreads = atoi(argv[1]);
    cant_x_thread = atoi(argv[2]);
    printf("n: %d\n", nthreads);
    pthread_t t[nthreads];
    int id[nthreads];

    init(nthreads);
    for (int i = 0; i < nthreads; i++)
        id[i] = i;

    for (int i = 0; i < nthreads; i++)
        pthread_create(t + i, NULL, molinete, (void*) (id + i));
    for (int i = 0; i < nthreads; i++)
        pthread_join(t[i], NULL);

    cleanup();
    printf("Visitantes: %d\n", visitantes);
    return 0;
}
