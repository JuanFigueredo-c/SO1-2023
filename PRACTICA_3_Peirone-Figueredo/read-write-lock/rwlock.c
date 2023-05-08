#include "rwlock.h"
#include <stdlib.h>
#include <semaphore.h>

struct _RWLock {
  sem_t read_count_lock, lock;
  sem_t torniquete;
  unsigned readers;
};

rwlock_t rwlock_init() {
  rwlock_t rwlock = malloc(sizeof(struct _RWLock));
  sem_init(&rwlock->read_count_lock, 0, 1);
  sem_init(&rwlock->lock, 0, 1);
  sem_init(&rwlock->torniquete, 0, 1);
  rwlock->readers = 0;
  return rwlock;
}

void rwlock_destroy(rwlock_t rwlock) {
  sem_destroy(&rwlock->read_count_lock);
  sem_destroy(&rwlock->lock);
  sem_destroy(&rwlock->torniquete);
  free(rwlock);
}

void read_lock(rwlock_t rwlock) {
  sem_wait(&rwlock->torniquete);
  sem_post(&rwlock->torniquete);
  sem_wait(&rwlock->read_count_lock);
  if (++rwlock->readers == 1)
    sem_wait(&rwlock->lock);
  sem_post(&rwlock->read_count_lock);
}

void read_unlock(rwlock_t rwlock) {
  sem_wait(&rwlock->read_count_lock);
  if (--rwlock->readers == 0)
    sem_post(&rwlock->lock);
  sem_post(&rwlock->read_count_lock);
}

void write_lock(rwlock_t rwlock) {
  sem_wait(&rwlock->torniquete);
  sem_wait(&rwlock->lock);
  sem_post(&rwlock->torniquete);
}

void write_unlock(rwlock_t rwlock) {
  sem_post(&rwlock->lock);
}

