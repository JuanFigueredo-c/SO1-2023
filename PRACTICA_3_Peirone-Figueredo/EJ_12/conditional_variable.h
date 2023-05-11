#ifndef __CONDITIONAL_VARIABLE_H
#define __CONDITIONAL_VARIABLE_H

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct _CondVar {
    sem_t signal;
    sem_t internal_lock;
    int locked;
} *CondVar;

CondVar cv_init();

void cv_destroy(CondVar cv);

int cv_wait(CondVar cv, pthread_mutex_t* mutex);

int cv_signal(CondVar cv);

int cv_broadcast(CondVar cv);

#endif
