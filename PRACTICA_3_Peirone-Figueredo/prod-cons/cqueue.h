#ifndef __CQUEUE_H__ // Concurrent bounded queue
#define __CQUEUE_H__ 

// WARNING: Basic implementation, panics when popping
// out of empty queue or pushing to full queue
// It doesnt copy nor destroy data stored in it
#include <stdlib.h>

typedef struct _CQueue* CQueue;

CQueue cqueue_init(size_t size);

int cqueue_empty(CQueue cqueue);

int cqueue_full(CQueue cqueue);

// Concurrent checking first element of queue, ensures a thread safe top
void* cqueue_ctop(CQueue cqueue);

// Concurrent popping out of queue, ensures a thread safe pop
void* cqueue_cpop(CQueue cqueue);

// Concurrent pushing to queue, ensures a thread safe push 
void cqueue_cpush(CQueue cqueue, void* data);

void cqueue_destroy(CQueue cqueue);

#endif // __CQUEUE_H__ 
