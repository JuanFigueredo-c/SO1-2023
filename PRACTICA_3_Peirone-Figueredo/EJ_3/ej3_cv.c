#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "cstack.h"

#define PROD 3
#define CLI 3
#define BUF_SIZE 50

CStack *buf;

pthread_t productores[PROD];
pthread_t consumidores[CLI];

pthread_mutex_t mutex_producidos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_libres = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cv_producidos;
pthread_cond_t cv_libres;
void enviar(int* p) {
  conc_push(buf, p);
}

int* recibir() {
  return conc_pop(buf);
}

void* producir() {
  int* value;
  int index;
  while (1) {
    while(conc_isFull(buf)) {
      pthread_cond_wait(&cv_libres, &mutex_producidos);
    }
    value = malloc(sizeof(int));
    *value = rand() % 30;
    printf("valor escrito: %d\n", *value);
    enviar(value);
    pthread_cond_signal(&cv_producidos);
    pthread_mutex_unlock(&mutex_producidos);
  }
}

void* consumir() {
  int* value;
  while (1) {
    while(conc_isEmpty(buf)) { 
      pthread_cond_wait(&cv_producidos, &mutex_libres);
    }
    value = recibir();
    printf("valor recibido %d\n", *value);
    free(value);
    pthread_cond_signal(&cv_libres);
    pthread_mutex_unlock(&mutex_libres);
  }
}

int main() {
  buf = conc_stack_init(BUF_SIZE);
  pthread_mutex_init(&mutex_producidos, NULL);
  pthread_mutex_init(&mutex_libres, NULL);
  pthread_cond_init(&cv_producidos, NULL);
  pthread_cond_init(&cv_libres, NULL);
  for (int i = 0; i < PROD; i++)
    pthread_create(productores + i, NULL, producir, NULL);
  for (int i = 0; i < CLI; i++)
    pthread_create(consumidores + i, NULL, consumir, NULL);
  pthread_join(productores[0], NULL); /* Espera para siempre */
  pthread_cond_destroy(&cv_producidos);
  pthread_cond_destroy(&cv_libres);
  conc_stack_destroy(buf);
}  