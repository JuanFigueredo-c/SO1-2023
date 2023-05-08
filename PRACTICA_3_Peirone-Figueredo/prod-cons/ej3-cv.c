#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
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

/*
 * La variable de condicion non_empty_buf servira
 * para hacer esperar a consumidores que pueden leer
 * del buffer, mientras que non_full_buf sirve para
 * avisar a productores que pueden escribir
 */
pthread_cond_t non_empty_buf = PTHREAD_COND_INITIALIZER;
pthread_cond_t non_full_buf = PTHREAD_COND_INITIALIZER;
pthread_mutex_t cqueue_lock = PTHREAD_MUTEX_INITIALIZER;

void enviar(int *p) {
  pthread_mutex_lock(&cqueue_lock);
  while (cqueue_full(buf))
    pthread_cond_wait(&non_full_buf, &cqueue_lock);
  cqueue_cpush(buf, (void*) p);
  pthread_cond_signal(&non_empty_buf);
  pthread_mutex_unlock(&cqueue_lock);
}

int* recibir() {
  int* p;
  pthread_mutex_lock(&cqueue_lock);
  while (cqueue_empty(buf))
    pthread_cond_wait(&non_empty_buf, &cqueue_lock);
  p = (int*) cqueue_cpop(buf);
  pthread_cond_signal(&non_empty_buf);
  pthread_mutex_unlock(&cqueue_lock);
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
	pthread_t productores[PRODS], consumidores[CONS];

	for (int i = 0; i < PRODS; i++)
		pthread_create(productores + i, NULL, producir, i + (void*)0);

	for (int i = 0; i < CONS; i++)
		pthread_create(consumidores + i, NULL, consumir, i + (void*)0);

	pthread_join(productores[0], NULL); /* Espera para siempre */
  cqueue_destroy(buf);
	return 0;
}

