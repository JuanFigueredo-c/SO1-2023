// C program for linked list implementation of stack
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "cstack.h"
#include <pthread.h>
#include <assert.h>

// source: https://www.geeksforgeeks.org/stack-data-structure-introduction-program/
 

int isEmpty(CStack* cstack) {
    return (cstack->index == 0);
}
 
void push(CStack* cstack, int* data) {
    cstack->stack[cstack->index] = data;
    cstack->index ++;
}
 
int* pop(CStack* cstack) {
    if (isEmpty(cstack))
        assert(0);
    int* popped = cstack->stack[cstack->index - 1];
    cstack->index --;
    return popped;
}
 
int* top(CStack* cstack) {
    if(isEmpty(cstack))
        assert(0);
    return cstack->stack[0];
}

void stackFree(CStack* cstack) {
  while(!isEmpty(cstack)) {
	  pop(cstack);
  }
}

void conc_stack_destroy(CStack* cstack) {
    stackFree(cstack);
    sem_destroy(&cstack->sem);
    free(cstack->stack);
    free(cstack);
}

CStack *conc_stack_init (int stack_size) {
    CStack* cstack = malloc(sizeof(CStack));
    sem_init(&cstack->sem,0,1);
    cstack->stack = malloc(sizeof(int*) * stack_size);
    cstack->index = 0;
    return cstack;
}

void conc_push(CStack *cstack, int* data) {
    sem_wait(&cstack->sem);
    push(cstack, data);
    sem_post(&cstack->sem);
}

int* conc_pop(CStack *cstack) {
    sem_wait(&cstack->sem);
    int* item = pop(cstack);
    sem_post(&cstack->sem);
    return item;
}

