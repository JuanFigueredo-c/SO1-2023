#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "timing.h"
#ifndef N
#define N 500
#endif

int A[N][N], B[N][N], C[N][N];

void mult(int A[N][N], int B[N][N], int C[N][N]) {
    int i, j, k;
    int B2[N][N];
    #pragma omp parallel for
    for (i = 0; i < N; i++)
        #pragma omp parallel for
        for (j = 0; j < N; j++)
        #pragma omp parallel for 
            for (k = 0; k < N; k++){
                C[i][j] += A[i][k] * B[k][j];
            }
}

int main() {
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = random() % 1000;
            B[i][j] = random() % 1000;
        }
    }
    float* t1;
    mult(A,B,C);
    return 0;
}