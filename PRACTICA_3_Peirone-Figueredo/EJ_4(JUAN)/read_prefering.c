#include <stdlib.h>
#include <stdio.h>
#include "read_write_lock.h"

RWLock rwl_init() {
    RWLock rw = malloc(sizeof(struct _RWLock));
    sem_init(&rw->internal_lock , 0, 1);
    sem_init(&rw->mode_lock , 0, 1);
    rw->counter = 0;
    return rw;
}

int rwl_destroy(RWLock rw) {
    if (sem_destroy(&rw->internal_lock) != 0) return -1;
    if (sem_destroy(&rw->mode_lock) != 0) return -1;
    free(rw);
    return 0;
}

int rwl_readers(RWLock rw) {
    int value = -1;
    sem_wait(&rw->internal_lock);
    value = rw->counter;
    sem_post(&rw->internal_lock);
    return value;
}

int rwl_lock_writers(RWLock rw) {
    if(sem_wait(&rw->mode_lock) != 0) return -1;
    return 0;
}    

int rwl_lock_readers(RWLock rw) {
    if(rwl_readers(rw) == 0) {
        if(sem_wait(&rw->mode_lock) != 0) return -1; 
    }
    sem_wait(&rw->internal_lock);
    rw->counter ++;
    sem_post(&rw->internal_lock);
    return 0;
}

int rwl_unlock_readers(RWLock rw) {
    sem_wait(&rw->internal_lock);
    rw->counter --;
    if(rw->counter == 0) {
        if(sem_post(&rw->mode_lock) != 0) return -1;
    }
    sem_post(&rw->internal_lock);
    return 0;
}

int rwl_unlock_writers(RWLock rw) {
    if(sem_post(&rw->mode_lock) != 0) return -1;
    return 0;
}

