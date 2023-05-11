#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include "conditional_variable.h"

CondVar cv_init() {
    CondVar cv = malloc(sizeof(struct _CondVar));
    sem_init(&cv->signal, 0, 0);
    sem_init(&cv->internal_lock, 0, 1);
    cv->locked = 0;
    return cv;
}

void cv_destroy(CondVar cv) {
    sem_destroy(&cv->signal);
    sem_destroy(&cv->internal_lock);
    free(cv);
}

int cv_wait(CondVar cv, pthread_mutex_t* mutex) {
    sem_wait(&cv->internal_lock);
    cv->locked++;
    pthread_mutex_unlock(mutex);
    sem_post(&cv->internal_lock);
    sem_wait(&cv->signal);
    pthread_mutex_lock(mutex);
}

int cv_signal(CondVar cv) {
    sem_wait(&cv->internal_lock);
    if(cv->locked == 0) {
        sem_post(&cv->internal_lock);
        return 0;
    }
    sem_post(&cv->signal);
    cv->locked--;
    sem_post(&cv->internal_lock);
}

int cv_broadcast(CondVar cv) {
    sem_wait(&cv->internal_lock);
    if(cv->locked == 0) {
        sem_post(&cv->internal_lock);
        return 0;
    }
    for(int i = 0; i < cv->locked; i++) {
        sem_post(&cv->signal);
    }
    cv->locked = 0;
    sem_post(&cv->internal_lock);
}