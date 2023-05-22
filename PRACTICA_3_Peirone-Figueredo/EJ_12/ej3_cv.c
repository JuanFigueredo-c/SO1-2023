#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "cqueue.h"
#include "conditional_variable.h"

#define PROD 3
#define CLI 3
#define BUF_SIZE 50

CQueue buf;

pthread_t productores[PROD];
pthread_t consumidores[CLI];

pthread_mutex_t mutex_producidos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_libres = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t safe = PTHREAD_MUTEX_INITIALIZER;

CondVar cv_producidos;
CondVar cv_libres;

void enviar(int* p) {
  cqueue_cpush(buf, p);
}

int* recibir() {
  return cqueue_cpop(buf);
}

void* producir() {
  int* value;
  int index;
  while (1) {
    pthread_mutex_lock(&mutex_producidos);
    while(cqueue_full(buf)) {
      cv_wait(cv_libres, &mutex_producidos);
    }
    value = malloc(sizeof(int));
    *value = rand() % 30;
    enviar(value);
    printf("valor escrito: %d\n", *value);
    //pthread_mutex_unlock(&safe);
    cv_signal(cv_producidos);
    pthread_mutex_unlock(&mutex_producidos);
  }
}

void* consumir() {
  int* value;
  while (1) {
    pthread_mutex_lock(&mutex_libres);
    while(cqueue_empty(buf)) { 
      cv_wait(cv_producidos, &mutex_libres);
    }
    value = recibir();
    printf("valor recibido %d\n", *value);
    free(value);
    //pthread_mutex_unlock(&safe);
    cv_signal(cv_libres);
    pthread_mutex_unlock(&mutex_libres);
  }
}

int main() {
  buf = cqueue_init(BUF_SIZE);
  cv_producidos = cv_init();
  cv_libres = cv_init();
  for (int i = 0; i < PROD; i++)
    pthread_create(productores + i, NULL, producir, NULL);
  for (int i = 0; i < CLI; i++)
    pthread_create(consumidores + i, NULL, consumir, NULL);
  pthread_join(productores[0], NULL); /* Espera para siempre */
  cv_destroy(cv_producidos);
  cv_destroy(cv_libres);
  cqueue_destroy(buf);
}  