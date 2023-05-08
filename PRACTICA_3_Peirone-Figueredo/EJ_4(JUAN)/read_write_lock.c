#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "read_write_lock.h"

ReadWriteLock* rw_init() {
    ReadWriteLock* rw = malloc(sizeof(ReadWriteLock));
    sem_init(&(rw->internal_lock), 0, 1);
    sem_init(&(rw->write_lock), 0, 1);
    sem_init(&(rw->read_lock), 0, 0);
    rw->readers = 0;
    rw->writers = 0;
    return rw;
}

void rw_destroy(ReadWriteLock* rw) {
    sem_destroy(&rw->internal_lock);
    sem_destroy(&rw->write_lock);
    sem_destroy(&rw->read_lock);
    free(rw);
}

int rw_add_reader(ReadWriteLock* rw) {
    if(sem_wait(&rw->internal_lock) != 0) return -1;
    rw->readers ++;
    if(sem_post(&rw->internal_lock) != 0) return -1;
    return 0;
}

int rw_del_reader(ReadWriteLock* rw) {
    if(sem_wait(&rw->internal_lock) != 0) return -1;
    rw->readers --;
    if(sem_post(&rw->internal_lock)!= 0) return -1;
    return 0;
}

int rw_add_writer(ReadWriteLock* rw) {
    if(sem_wait(&rw->internal_lock) != 0) return -1;
    rw->writers ++;
    if(sem_post(&rw->internal_lock) != 0) return -1;
    return 0;
}

int rw_del_writer(ReadWriteLock* rw) {
    if(sem_wait(&rw->internal_lock) != 0) return -1;
    rw->writers --;
    if(sem_post(&rw->internal_lock) != 0) return -1;
    return 0;
}

int rw_lock_readers(ReadWriteLock* rw) {
    return sem_wait(&rw->read_lock);
}

int rw_unlock_readers(ReadWriteLock* rw) {
    return sem_post(&rw->read_lock);
}

int rw_lock_writers(ReadWriteLock* rw) {
    return sem_wait(&rw->write_lock);
} 

int rw_unlock_writers(ReadWriteLock* rw) {
    return sem_post(&rw->write_lock);
}
int rw_unlock_w_rp(ReadWriteLock* rw) {
    int value = -1;
    if(rw_readers(rw) == 0) {
        value = rw_unlock_writers(rw);
    }
    return value;
}

int rw_lock_r_wp(ReadWriteLock* rw) {
    int value = -1;
    if(rw_writers != 0) {
        sem_wait(&rw->internal_lock);
        rw->readers = 0;
        sem_post(&rw->internal_lock);
        rw_unlock_writers(rw);
        return rw_lock_readers(rw);
    }    
}
int rw_readers(ReadWriteLock* rw) {
    int value;
    if(sem_wait(&rw->internal_lock) != 0) return -1;
    value = rw->readers;
    if(sem_post(&rw->internal_lock)!= 0) return -1;
    return value;
}


int rw_writers(ReadWriteLock* rw) {
    int value;
    if(sem_wait(&rw->internal_lock) != 0) return -1;
    value = rw->writers;
    if(sem_post(&rw->internal_lock) != 0) return -1;
    return value;
}