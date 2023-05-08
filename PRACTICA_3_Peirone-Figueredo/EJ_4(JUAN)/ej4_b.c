#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "read_write_lock.h"

#define M 1
#define N 10
#define ARRLEN 10240

RWL rw;

int arr[ARRLEN];
void * escritor(void *arg) {
    int i;
    int num = arg - (void*)0;
    while (1) {
        sleep(random() % 3);
        rw_add_writer(rw);
        rw_lock_writers(rw);
        printf("Escritor %d escribiendo\n", num);
        for (i = 0; i < ARRLEN; i++)
        arr[i] = num;
        rw_del_writer(rw);
        rw_unlock_readers(rw);
        rw_unlock_writers(rw);
    }
    return NULL;
}

void * lector(void *arg) {
    int v, i;
    int num = arg - (void*)0;
    while (1) {
        sleep(random() % 3);
        rw_lock_r_wp(rw);
        if(rw_readers(rw) == 0) { 
            rw_lock_writers(rw);
        }
        //rw_add_reader(rw);
        v = arr[0];
        for (i = 1; i < ARRLEN; i++) {
            if (arr[i] != v)
            break;
        }
        if (i < ARRLEN)
            printf("Lector %d, error de lectura\n", num);
        else
            printf("Lector %d, dato %d\n", num, v);
        //rw_del_reader(rw);
        //rw_unlock_writers(rw);
    }
    return NULL;
}

int main() {
    pthread_t lectores[M], escritores[N];
    int i;
    rw = rw_init();
    for (i = 0; i < M; i++)
        pthread_create(&lectores[i], NULL, lector, i + (void*)0);
    for (i = 0; i < N; i++)
        pthread_create(&escritores[i], NULL, escritor, i + (void*)0);
    pthread_join(lectores[0], NULL); /* Espera para siempre */
    rw_destroy(rw);
    return 0;
}