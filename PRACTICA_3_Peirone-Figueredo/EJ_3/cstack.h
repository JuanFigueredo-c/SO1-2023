#ifndef CONCURRENT_STACK_H
#define CONCURRENT_STACK_H

#include <stdlib.h>
#include <semaphore.h>

// A structure to represent a stack
// here we use a linked list to represent the unbound stack

typedef struct {
    int** stack;
    int index;
    sem_t sem;
    int limit;
} CStack;

CStack *conc_stack_init (int stack_size);

int isEmpty(CStack *cstack);

int isFull(CStack *cstack);
 
void push(CStack *cstack, int* data);
 
int* pop(CStack *cstack);
 
int* top(CStack *cstack);

void stackFree(CStack *cstack);

int* conc_pop (CStack *cstack);

void conc_push(CStack *cstack, int* data);

void conc_stack_destroy(CStack* cstack);

int conc_isEmpty(CStack *cstack);

int conc_isFull(CStack *cstack);
#endif /* CONCURRENT_STACK_H */