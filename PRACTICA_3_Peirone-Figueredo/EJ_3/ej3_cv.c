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

sem_t sem_producidos;
sem_t sem_libres;

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
    sleep(1);
    sem_wait(&sem_libres);
    value = malloc(sizeof(int));
    *value = rand() % 30;
    printf("valor escrito: %d\n", *value);
    enviar(value);
    sem_post(&sem_producidos);
  }
}

void* consumir() {
  int* value;
  while (1) {
    sleep(1);
    sem_wait(&sem_producidos);
    value = recibir();
    printf("valor recibido %d\n", *value);
    free(value);
    sem_post(&sem_libres);
  }
}

int main() {
  buf = conc_stack_init(BUF_SIZE);
  sem_init(&sem_producidos, 0, 0);
  sem_init(&sem_libres, 0, BUF_SIZE);
  for (int i = 0; i < PROD; i++)
    pthread_create(productores + i, NULL, producir, NULL);
  for (int i = 0; i < CLI; i++)
    pthread_create(consumidores + i, NULL, consumir, NULL);
  pthread_join(productores[0], NULL); /* Espera para siempre */
  conc_stack_destroy(buf);
  sem_destroy(&sem_producidos);
  sem_destroy(&sem_libres);
}  