#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/*
 * La idea general de la implementacion es que un fumador espere para ver
 * si sus recursos estan disponibles. Al entrar, usa un sem_trywait para
 * ver si su primer recurso esta disponible. Si no lo esta, da lugar a que otro
 * fumador pruebe. Si el primer recurso esta disponible, prueba con el segundo, y si
 * lo esta puede fumar. Si su segundo recurso no esta disponible, devuelve el primero
 * y nuevamente da lugar a que otro fumador intente. 
*/

sem_t tabaco, papel, fosforos, otra_vez, decidiendo;

void agente() {
  while (1) {
    sem_wait(&otra_vez);
    int caso = random() % 3;
    if (caso != 0) sem_post(&fosforos);
    if (caso != 1) sem_post(&papel);
    if (caso != 2) sem_post(&tabaco);
  }
}

void fumar(int fumador) {
  printf("Fumador %d: Puf! Puf! Puf!\n", fumador);
  sleep(1);
}

void* fumador1(void *arg) {
  while (1) {
    sem_wait(&decidiendo); // Prueba si sus recursos estan disponibles
    if (sem_trywait(&papel) != 0) { // Comprueba si su primer recurso esta disponible
      sem_post(&decidiendo); // Deja que otro fumador pruebe
      continue;
    }
    if (sem_trywait(&tabaco) != 0) { // Comprueba si su segundo recurso esta disponible
      sem_post(&papel); // Devuelve el primer recurso
      sem_post(&decidiendo); // Deja que otro fumador pruebe
      continue;
    }
    fumar(1);
    sem_post(&decidiendo);
    sem_post(&otra_vez);
  }
}

// Analogo para los demas fumadores
void* fumador2(void *arg) {
  while (1) {
    sem_wait(&decidiendo);
    if (sem_trywait(&tabaco) != 0) {
      sem_post(&decidiendo);
      continue;
    }
    if (sem_trywait(&fosforos) != 0) {
      sem_post(&tabaco);
      sem_post(&decidiendo);
      continue;
    }
    fumar(2);
    sem_post(&decidiendo);
    sem_post(&otra_vez);
  }
}

void* fumador3(void *arg) {
  while (1) {
    sem_wait(&decidiendo);
    if (sem_trywait(&fosforos) != 0) {
      sem_post(&decidiendo);
    }
    if (sem_trywait(&papel) != 0) {
      sem_post(&fosforos);
      sem_post(&decidiendo);
      continue;
    }
    fumar(3);
    sem_post(&decidiendo);
    sem_post(&otra_vez);
  }
}

int main() {
  pthread_t s1, s2, s3;
  sem_init(&tabaco, 0, 0);
  sem_init(&papel, 0, 0);
  sem_init(&fosforos, 0, 0);
  sem_init(&otra_vez, 0, 1);
  sem_init(&decidiendo, 0, 1);
  pthread_create(&s1, NULL, fumador1, NULL);
  pthread_create(&s2, NULL, fumador2, NULL);
  pthread_create(&s3, NULL, fumador3, NULL);
  agente();
  return 0;
}
