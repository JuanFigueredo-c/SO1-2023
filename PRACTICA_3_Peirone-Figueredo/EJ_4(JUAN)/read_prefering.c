#include <stdlib.h>
#include <stdio.h>
#include "read_prefering.h"

RWLock rwl_init() {
    RWLock rw = malloc(sizeof(struct _RWLock));
    sem_init(&rw->internal_lock , 0, 1);
    sem_init(&rw->readers_lock , 0, 1);
    rw->current_readers = 0;
    return rw;
}

int rwl_destroy(RWLock rw) {
    if (sem_destroy(&rw->internal_lock) != 0) return -1;
    if (sem_destroy(&rw->readers_lock) != 0) return -1;
    free(rw);
    return 0;
}

int rwl_readers(RWLock rw) {
    int value = -1;
    sem_wait(&rw->internal_lock);
    value = rw->current_readers;
    sem_post(&rw->internal_lock);
    return value;
}

int rwl_lock_writers(RWLock rw) {
    if(sem_wait(&rw->readers_lock) != 0) return -1;
    return 0;
}    

int rwl_lock_readers(RWLock rw) {
    if(rwl_readers(rw) == 0) {
        if(sem_wait(&rw->readers_lock) != 0) return -1; 
    }
    sem_wait(&rw->internal_lock);
    rw->current_readers ++;
    sem_post(&rw->internal_lock);
    return 0;
}

int rwl_unlock_readers(RWLock rw) {
    sem_wait(&rw->internal_lock);
    rw->current_readers --;
    if(rw->current_readers == 0) {
        if(sem_post(&rw->readers_lock) != 0) return -1;
    }
    sem_post(&rw->internal_lock);
    return 0;
}

int rwl_unlock_writers(RWLock rw) {
    if(sem_post(&rw->readers_lock) != 0) return -1;
    return 0;
}

