#include "../../timing.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 500

int A[N][N], B[N][N], C[N][N];

// Variables para manejar paralelismo anidado por niveles
int parallel_lvl[3] = {1, 1, 1};

/* Performs C = A * B */
void mult(int A[N][N], int B[N][N], int C[N][N]) {
  int i, j, k;
  #pragma omp parallel for if(parallel_lvl[0])
  for (i = 0; i < N; i++) {
    #pragma parallel omp for if(parallel_lvl[1])
    for (j = 0; j < N; j++) {
      #pragma parallel omp for if(parallel_lvl[2])
      for (k = 0; k < N; k++) {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}

/* Performs C = A * B */
/* Optimized version to decrease cache misses */
void mult_opt(int A[N][N], int B[N][N], int C[N][N]) {
  int i, j, k;
  #pragma omp parallel for if(parallel_lvl[0])
  for (i = 0; i < N; i++)
    #pragma omp parallel for if(parallel_lvl[1])
    for (k = 0; k < N; k++)
      #pragma omp parallel for if(parallel_lvl[2])
      for (j = 0; j < N; j++)
        C[i][j] += A[i][k] * B[k][j];
}

/* Performs C = A * B^T */
void mult_transpose(int A[N][N], int B[N][N], int C[N][N]) {
  int i, j, k;
  #pragma omp parallel for if(parallel_lvl[0])
  for (i = 0; i < N; i++)
    #pragma omp parallel for if(parallel_lvl[1])
    for (j = 0; j < N; j++)
      #pragma omp parallel for if(parallel_lvl[2])
      for (k = 0; k < N; k++)
        C[i][j] += A[i][k] * B[j][k];
}

int main() {
  int i, j;
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      A[i][j] = random() % 1000;
      B[i][j] = random() % 1000;
    }
  }
  omp_set_num_threads(4);
  TIME_void(mult(A, B, C), NULL);
  TIME_void(mult_opt(A, B, C), NULL);
  TIME_void(mult_transpose(A, B, C), NULL);
  return 0;
}

