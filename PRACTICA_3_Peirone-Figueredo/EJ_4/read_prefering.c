#include <stdlib.h>
#include <stdio.h>
#include "read_write_lock.h"

struct _RWLock {
    sem_t internal_lock; // semaforo interno para manejo de concurrencias de la variable interna
    sem_t mode_lock; // semaforo dedicado para mantener concurrencia de la escritura y lectura
    int counter; // contador interno de la estructura, en esta implementacion lleva la cuenta de los lectores
};

// Inicializa la estructura utilizada para el read-write lock
RWLock rwl_init() {
    RWLock rw = malloc(sizeof(struct _RWLock)); 
    sem_init(&rw->internal_lock , 0, 1);
    sem_init(&rw->mode_lock , 0, 1);
    rw->counter = 0;
    return rw;
}

// Destruye la estructura utilizada para el read-write lock
void rwl_destroy(RWLock rw) {
    sem_destroy(&rw->internal_lock);
    sem_destroy(&rw->mode_lock);
    free(rw);
}

// Devuelve el contador interno de la estructura, que esta implementacion lleva la cuenta de los 
// lectores.
int rwl_readers(RWLock rw) {
    sem_wait(&rw->internal_lock);
    int value = rw->counter;
    sem_post(&rw->internal_lock);
    return value;
}

// Realiza un lock de los lectores
void rwl_lock_readers(RWLock rw) {
    if(rwl_readers(rw) == 0) {  // revisa si hay algun lector leyendo, si lo hay ingresa a leer directamente
        sem_wait(&rw->mode_lock); // si no hay ningun lector leyendo previamente, pide el lock (semaforo) y espera a poder tomarlo
    }
    sem_wait(&rw->internal_lock);
    rw->counter ++; // Cuando el lector ingresa a leer, aumenta concurrentemente el contador interno.
    sem_post(&rw->internal_lock);
}

// Realiza un unlock de los lectores
void rwl_unlock_readers(RWLock rw) {
    sem_wait(&rw->internal_lock);
    rw->counter --;  // Al terminar de leer, decrementa concurrentemente el contador interno.
    if(rw->counter == 0) {   // El lector revisa si fue el ultimo lector que quedo leyendo 
        sem_post(&rw->mode_lock);  // Si lo fue, libera el lock de la escritura y lectura.
    }
    sem_post(&rw->internal_lock);
}

// Realiza un lock de los escritores
void rwl_lock_writers(RWLock rw) {
    sem_wait(&rw->mode_lock);  // Espera el lock de escritura y lectura.
}    

// Realiza un unlock de los escritores
void rwl_unlock_writers(RWLock rw) {
    (sem_post(&rw->mode_lock) != 0);  // Libera el lock de escritura y lectura.
}

