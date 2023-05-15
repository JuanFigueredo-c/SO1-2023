#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "write_prefering.h"

RWLock rwl_init() {
    RWLock rw = malloc(sizeof(struct _RWLock));
    sem_init(&rw->internal_lock , 0, 1);
    sem_init(&rw->writers_lock , 0, 1);
    sem_init(&rw->no_writers , 0, 1);
    rw->writers = 0;
    return rw;
}

int rwl_destroy(RWLock rw) {
    if (sem_destroy(&rw->internal_lock) != 0) return -1;
    if (sem_destroy(&rw->writers_lock) != 0) return -1;
    if (sem_destroy(&rw->no_writers) != 0) return -1;
    free(rw);
    return 0;
}

int rwl_writers(RWLock rw) {
    int value = -1;
    sem_wait(&rw->internal_lock);
    value = rw->writers;
    sem_post(&rw->internal_lock);
    return value;
}


int rwl_lock_readers(RWLock rw) {
    if(rwl_writers(rw) > 0) {
        if(sem_wait(&rw->no_writers) != 0) return -1;
    }
    else{
        if(sem_wait(&rw->writers_lock) != 0) return -1;
    }
    return 0;
}

int rwl_unlock_readers(RWLock rw) {
    if(sem_post(&rw->writers_lock) != 0) return -1;
    return 0;
}    

int rwl_lock_writers(RWLock rw) {
    sem_wait(&rw->internal_lock);
    rw->writers ++;
    sem_post(&rw->internal_lock);
    sem_wait(&rw->writers_lock);
}

int rwl_unlock_writers(RWLock rw) {
    sem_wait(&rw->internal_lock);
    rw->writers--;
    if(rw->writers == 0)
        sem_post(&rw->no_writers);
    sem_post(&rw->internal_lock);
    sem_post(&rw->writers_lock);
    return 0;
}
