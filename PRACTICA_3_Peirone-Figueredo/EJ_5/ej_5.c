#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include "cqueue.h"
#include <stdlib.h>

#define SILLAS 5
#define N 10

pthread_mutex_t atender = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t disponible = PTHREAD_COND_INITIALIZER;

sem_t hay_clientes;
sem_t esperar_barbero;
sem_t esperar_cliente;
sem_t queue_lock;

int id_atendido;

CQueue queue;

void cortando(int id) {
    printf("cortando al cliente: %d\n",id);
    sleep(1);
}

void me_cortan(int id) {
    printf("soy el cliente cliente %d me estan cortando\n",id);
    sleep(1);
}

void pagando(int id) {
    printf("soy el cliente %d estoy pagando.\n",id);
}

void me_pagan(int id) {
    printf("el cliente %d me esta pagando \n",id);
}

void barber_sync(){
    sem_post(&esperar_barbero);
    sem_wait(&esperar_cliente);
}

void client_sync(){
    sem_post(&esperar_cliente);
    sem_wait(&esperar_barbero);
}

void* barber() {
    while(1) {
        sem_wait(&hay_clientes); // barbero durmiendo... 
        pthread_cond_broadcast(&disponible);
        cortando(id_atendido);
        barber_sync();
        me_pagan(id_atendido);
        barber_sync();
    }
}

void* client(void* arg){
    int id = arg - NULL;
    while(1){
        sleep(rand() % 3);
        printf("entra el cliente %d\n",id);
        sem_wait(&queue_lock);
        if(cqueue_full(queue)) {
            printf("no hay lugar, el cliente %d se va\n",id);
            sem_post(&queue_lock);
            continue;
        }
        cqueue_cpush(queue, (size_t) id);
        sem_post(&hay_clientes);
        sem_post(&queue_lock);
        pthread_mutex_lock(&atender);
        while((size_t) cqueue_ctop(queue) != id){
            pthread_cond_wait(&disponible, &atender);
        }
        id_atendido = (size_t) cqueue_cpop(queue);
        me_cortan(id);
        client_sync();
        pagando(id);
        client_sync();
        pthread_mutex_unlock(&atender);
    }
}

int main() {
    queue = cqueue_init(SILLAS);
    sem_init(&hay_clientes, 0,0);;
    sem_init(&esperar_barbero, 0,0);
    sem_init(&esperar_cliente, 0,0);
    sem_init(&queue_lock, 0,1);
    pthread_t barber_thread, clients_thread[N];
    pthread_create(&barber_thread, NULL, barber, NULL);
    for (int i = 0; i < N; i++) {
        pthread_create(clients_thread + i, NULL, client, i + NULL);
    }
    pthread_join(barber_thread,NULL);
    cqueue_destroy(queue);
}