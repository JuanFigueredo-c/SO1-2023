#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);
    int my_id, cant_proccess;
  
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &cant_proccess);

    int my_sum = my_id;       // Se tomó al identificador de cada proceso como la variable privada
    int recv_num, aux_num;    
    int next_proccess = (my_id + 1) % cant_proccess;
    int prev_proccess = my_id == 0 ? (cant_proccess - 1) : (my_id - 1);
    aux_num = my_sum;
    MPI_Barrier(MPI_COMM_WORLD);   // Sincronización de todos los procesos, para que obtengan sus datos antes de empezar
    for(int i = 0; i < (cant_proccess-1) ; i ++) {
        MPI_Send(&aux_num,1,MPI_INT,next_proccess,0,MPI_COMM_WORLD);
        MPI_Recv(&recv_num,1,MPI_INT,prev_proccess,0,MPI_COMM_WORLD, NULL);
        my_sum = my_sum + recv_num;
        aux_num = recv_num;
        MPI_Barrier(MPI_COMM_WORLD);   
    }
    printf("La suma total es: %d\n",my_sum);
    MPI_Finalize();
}
