#include "lamport.h"
#include <assert.h>
#include <stdlib.h>

int *flag;
int *choose; // choose[i] == 1 si i esta eligiendo maximo

void init(int nthreads) {
    choose = malloc(sizeof(int) * nthreads);
    flag = malloc(sizeof(int) * nthreads);
    assert(choose && flag);
    for (int i = 0; i < nthreads; i++) {
        choose[i] = 0;
        flag[i] = 0;
    }
}

void cleanup() {
    free(choose);
    free(flag);
}


static inline int max(int* v, int n) {
  int result = v[0];
    for (int i = 1; i < n; i++)
        if (v[i] > result)
            result = v[i];
    return result;
}

void lock(int id, int nthreads) {
    choose[id] = 1;
    asm("mfence");
    flag[id] = 1 + max(flag, nthreads);
    choose[id] = 0;
    asm("mfence");
    for (int j = 0; j < nthreads; j++) {
        while (choose[j]);
        while (flag[j] != 0 && (flag[j] < flag[id] || (flag[j] == flag[id] && j < id)));
    }
}

void unlock(int id) {
    flag[id] = 0;
}
