#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include "cqueue.h"
#include <stdlib.h>

#define SILLAS 5
#define N 10

pthread_mutex_t atender = PTHREAD_MUTEX_INITIALIZER; // Mutex utilizado para la variable de condicion "disponible"
                                                     // indica a los clientes que el barbero esta listo para atender
pthread_cond_t disponible = PTHREAD_COND_INITIALIZER; // Variable de condicion que utiliza el mutex "atender". Despierta
                                                      // a los clientes que estan esperando ser atendidos

sem_t hay_clientes;  // Semaforo utilizado para despertar al barbero, si este esta bajo el barbero duerme, cuando
                     // entra un cliente, el semaforo se sube.
sem_t esperar_barbero; // Semaforo utilizado para sincronizar el cliente y el barbero en las etapas de cortar y pagar. 
sem_t esperar_cliente; // Semaforo utilizado para sincronizar el cliente y el barbero en las etapas de cortar y pagar.
sem_t queue_lock; // Semafoto utilizado para atomizar los procesos en los cuales el cliente revisa la cola para
                  // identificar si puede entrar o para agregarse a la misma

int id_atendido; // Almacena el id del cliente atendido actual

CQueue queue; // Cola concurrente

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

// Funcion utilizada para sincronizar el barbero con el cliente
void barber_sync(){
    sem_post(&esperar_barbero);
    sem_wait(&esperar_cliente);
}

// Funcion utilizada para sincronizar el barbero con el cliente
void client_sync(){
    sem_post(&esperar_cliente);
    sem_wait(&esperar_barbero);
}

void* barber() {
    while(1) {
        sem_wait(&hay_clientes); // barbero durmiendo... 
        pthread_cond_broadcast(&disponible);  // Despierta a todos los clientes esperando su turno
        cortando(id_atendido); // Atiende al primer cliente de la cola
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
        if(cqueue_full(queue)) { // Chequeo concurrente de la disponibilidad en la cola
            printf("no hay lugar, el cliente %d se va\n",id);
            sem_post(&queue_lock);
            continue;
        }
        cqueue_cpush(queue, (size_t) id); // Si hay lugar, el cliente se agrega a la cola
        sem_post(&hay_clientes); // Despierta al barbero
        sem_post(&queue_lock);  
        pthread_mutex_lock(&atender);
        while((size_t) cqueue_ctop(queue) != id){     // Los clientes esperaran hasta que el barbero haga un broadcast
            pthread_cond_wait(&disponible, &atender); // Si se despiertan y su id coincide con el tope de la cola  
        }                                             // se despiertan y toman el mutex "atender", sino vuelven a dormir.
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