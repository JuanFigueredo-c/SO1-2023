#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "rwlock.h"

#define M 5 // Numero de lectores
#define N 5 // Numero de escritores
#define ARRLEN 10240

int arr[ARRLEN];
rwlock_t rwlock;

void* escritor(void *arg) {
  int i;
  int num = arg - (void*)0;
  while (1) {
    sleep(random() % 2);
    write_lock(rwlock);
    printf("Escritor %d escribiendo\n", num);
    exit(0);
    for (i = 0; i < ARRLEN; i++)
      arr[i] = num;
    write_unlock(rwlock);
  }
  return NULL;
}

void* lector(void *arg) {
  int v, i;
  int num = arg - (void*)0;
  while (1) {
    //sleep(random() % 2);
    read_lock(rwlock);
    v = arr[0];
    for (i = 1; i < ARRLEN; i++) {
      if (arr[i] != v)
        break;
    }
    read_unlock(rwlock);
    if (i < ARRLEN)
      printf("Lector %d, error de lectura\n", num);
    else
      printf("Lector %d, dato %d\n", num, v);
  }
  return NULL;
}

int main() {
  rwlock = rwlock_init();
  pthread_t lectores[M], escritores[N];
  int i;
  for (i = 0; i < M; i++)
    pthread_create(&lectores[i], NULL, lector, i + (void*)0);
  for (i = 0; i < N; i++)
    pthread_create(&escritores[i], NULL, escritor, i + (void*)0);
  
  pthread_join(lectores[0], NULL); /* Espera para siempre */
  rwlock_destroy(rwlock);
  return 0;
}

