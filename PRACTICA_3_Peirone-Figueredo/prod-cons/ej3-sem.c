#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "cqueue.h"

#define PRODS 5
#define CONS 5
#define SZ 8

/*
 * El buffer guarda punteros a enteros, los
 * productores los consiguen con malloc() y los
 * consumidores los liberan con free()
 */
CQueue buf;

// sem_producidos indica la cantidad de objetos
// disponibles para consumir, sem_disponibles la cantidad
// de espacio en el buffer
sem_t sem_producidos, sem_disponibles;

void enviar(int *p) {
  sem_wait(&sem_disponibles);
  cqueue_cpush(buf, (void*) p);
  sem_post(&sem_producidos);
}

int* recibir() {
  int* p;
  sem_wait(&sem_producidos);
  p = (int*) cqueue_cpop(buf);
  sem_post(&sem_disponibles);
	return p;
}

void* producir(void *arg) {
	int id = arg - (void*)0;
	while (1) {
		sleep(random() % 3);

		int *p = malloc(sizeof *p);
		*p = random() % 100;
		printf("Productor %d: produje %p->%d\n", id, p, *p);
		enviar(p);
	}
	return NULL;
}

void* consumir(void *arg) {
	int id = arg - (void*)0;
	while (1) {
		sleep(random() % 3);

		int *p = recibir();
		printf("Consumidor %d: obtuve %p->%d\n", id, p, *p);
		free(p);
	}
	return NULL;
}

int main() {
  buf = cqueue_init(SZ);
  sem_init(&sem_producidos, 0, 0);
  sem_init(&sem_disponibles, 0, SZ);
	pthread_t productores[PRODS], consumidores[CONS];

	for (int i = 0; i < PRODS; i++)
		pthread_create(productores + i, NULL, producir, i + (void*)0);

	for (int i = 0; i < CONS; i++)
		pthread_create(consumidores + i, NULL, consumir, i + (void*)0);

	pthread_join(productores[0], NULL); /* Espera para siempre */
  cqueue_destroy(buf);
	return 0;
}

