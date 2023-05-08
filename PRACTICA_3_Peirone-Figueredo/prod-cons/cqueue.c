#include "cqueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

struct _CQueue {
  int top, last;
  int size;
  int full;
  void** array;
  sem_t lock;
};

CQueue cqueue_init(size_t size) {
  CQueue cqueue = malloc(sizeof(struct _CQueue));
  cqueue->array = malloc(sizeof(void*) * size);
  cqueue->size = size;
  cqueue->full = 0;
  cqueue->top = cqueue->last = 0;
  sem_init(&cqueue->lock, 0, 1);
  return cqueue;
}

// Not concurrent checking of empty queue,
// used to avoid locking twice a mutex by same thread
int _cqueue_empty_nc(CQueue cqueue) {
  return cqueue->top == cqueue->last;
}

// Not concurrent checking of full queue,
// used to avoid locking twice a mutex by same thread
int _cqueue_full_nc(CQueue cqueue) {
  return cqueue->full;
}

int cqueue_empty(CQueue cqueue) {
  int b;
  sem_wait(&cqueue->lock);
  cqueue->top == cqueue->last;
  sem_post(&cqueue->lock);
  return b;
}

int cqueue_full(CQueue cqueue) {
  int b;
  sem_wait(&cqueue->lock);
  b = cqueue->full;
  sem_post(&cqueue->lock);
  return b;
}

void* cqueue_cpop(CQueue cqueue) {
  sem_wait(&cqueue->lock);
  if (_cqueue_empty_nc(cqueue))
    abort();
  void* top = cqueue->array[cqueue->top];
  cqueue->top = (cqueue->top + 1) % cqueue->size;
  cqueue->full = 0;
  sem_post(&cqueue->lock);
  return top;
}

void cqueue_cpush(CQueue cqueue, void* data) {
  sem_wait(&cqueue->lock);
  if (_cqueue_full_nc(cqueue))
    abort();
  cqueue->array[cqueue->last] = data;
  cqueue->last = (cqueue->last + 1) % cqueue->size;
  cqueue->full = cqueue->last == cqueue->top;
  sem_post(&cqueue->lock);
}

void cqueue_destroy(CQueue cqueue) {
  free(cqueue->array);
  sem_destroy(&cqueue->lock);
  free(cqueue);
}

