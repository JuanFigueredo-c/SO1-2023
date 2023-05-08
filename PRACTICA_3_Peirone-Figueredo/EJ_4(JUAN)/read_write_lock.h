#ifndef READ_WRITE_LOCK_H
#define READ_WRITE_LOCK_H

#include <stdlib.h>
#include <semaphore.h>

typedef struct{
    sem_t internal_lock;
    sem_t write_lock;
    sem_t read_lock;
    int readers;
    int writers;
} ReadWriteLock;

typedef ReadWriteLock* RWL;

ReadWriteLock* rw_init();

void rw_destroy(ReadWriteLock* rw);

int rw_add_reader(ReadWriteLock* rw);

int rw_del_reader(ReadWriteLock* rw);

int rw_add_writer(ReadWriteLock* rw);

int rw_del_writer(ReadWriteLock* rw);

int rw_lock_readers(ReadWriteLock* rw);
    
int rw_unlock_readers(ReadWriteLock* rw);

int rw_lock_writers(ReadWriteLock* rw);

int rw_unlock_writers(ReadWriteLock* rw);

int rw_unlock_w_rp(ReadWriteLock* rw);

int rw_lock_r_wp(ReadWriteLock* rw);

int rw_readers(ReadWriteLock* rw);

int rw_writers(ReadWriteLock* rw);

#endif