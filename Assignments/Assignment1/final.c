// a1810750 

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    //initialising
    int comm_sz; // Number of processes 
    int my_rank; // My process rank 
    int token; // Token value
  
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Set the token value for each process equal to its proccess value
    // If the current process is 0, initilise all other proccesses. 
    for (int i = 0; i < comm_sz; i++){
        if (my_rank == 0) {
            token = i; 
            MPI_Send(&token, 1, MPI_INT, i, 0, MPI_COMM_WORLD); 
        }
    }

    // For each process complete a left shift 
    for (int j = 0; j < comm_sz - 1; j++){

        // If shifting from process 3, send the current token to process 0
        if (my_rank == 3){
            printf("Process %d sending token %d to process 0\n", my_rank, token);
            MPI_Send(&token, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }

        // Else send the current token to the next process
        else { 
            printf("Process %d sending token %d to process %d\n", my_rank, token, my_rank + 1);
            MPI_Send(&token, 1, MPI_INT, (my_rank + 1), 0, MPI_COMM_WORLD);
        }

        // If not working wihh process 0 receive a token from the previous process 
        if (my_rank != 0 ){
            MPI_Recv(&token, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d received token %d from process %d\n", my_rank, token, my_rank - 1);
        }

        // Else recieve a token from the previous process 
        else {
            MPI_Recv(&token, 1, MPI_INT, comm_sz - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d received token %d from process %d\n", my_rank, token, comm_sz - 1);
        }
    }

  MPI_Finalize();
  return 0;
}