#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <pthread.h>

#define NVISITANTES 1000000

int visitantes = 0;
int flag[2] = {0,0};
int turno = 0;

void* molinete0(void* arg) {
    for(int i = 0; i < NVISITANTES; i ++) {
        flag[0] = 1;
        turno = 1;
        asm("mfence");
        while(flag[1] == 1 && turno == 1) {}
        visitantes ++;
        flag[0] = 0;
    }
}

void* molinete1(void* arg) {
    for(int i = 0; i < NVISITANTES; i ++) {
        flag[1] = 1;
        turno = 0;
        asm("mfence");
        while(flag[0] == 1 && turno == 0) {}
        visitantes ++;
        flag[1] = 0;
    }
}

int main(){
    pthread_t t0, t1;
    pthread_create(&t0, NULL, molinete0 , NULL);
    pthread_create(&t1, NULL, molinete1 , NULL);
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);

    printf("%d\n",visitantes);
    return 0;
}