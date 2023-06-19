#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
  int np, rank, val, buf;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  buf = val = rank;
  int next = (rank + 1) % np;
  int prev = rank ? rank - 1 : np - 1;
  for (int i = 0; i < np - 1; i++) {
    MPI_Send(&buf, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    MPI_Recv(&buf, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, NULL);
    buf += val;
  }
  printf("Rank: %d, Sum: %d\n", rank, buf);
  MPI_Finalize();
  return 0;
}
