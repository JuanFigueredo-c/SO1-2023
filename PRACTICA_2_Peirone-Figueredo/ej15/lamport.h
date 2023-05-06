#ifndef __LAMPORT_H__
#define __LAMPORT_H__

void init(int nthreads);

void cleanup();

void lock(int id, int nthreads);

void unlock(int id);

#endif