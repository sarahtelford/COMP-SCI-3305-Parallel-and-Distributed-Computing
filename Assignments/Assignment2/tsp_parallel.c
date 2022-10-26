#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// declaring the matrix 
int * distanceMatrix;
int matrixSize; 
int bestCost = 999;
int cityNum = 999;
int curCity = 0;
int curCost = 0;
int bestPath[999];
bool logging = 0;
char buff[255];
FILE * log_file; 

// function to allocate the matricies 
int * allocateMatrix(int matrixSize) {
    return calloc(matrixSize, sizeof(int));
}

// function to set values in the visited cities matrix
void setMatrix(int r, int c, int value) {
    distanceMatrix[r * matrixSize + c] = value;
}

// function to get values in the visited cities matrix
int getMatrix(int r, int c) {
    return distanceMatrix[r * matrixSize + c];
}

void tsp(int curCity, int visited[], int curCost){
    int copyVisited[cityNum]; 
    int count = 0;      // counter for how many cities have been visited

    // create a copy of the visited cities array, to prevent issues with overriding in recursion
    for(int i = 0; i < cityNum; i++){
        copyVisited[i] = visited[i];
    }
    visited = copyVisited;

    //visit the current city
    visited[curCity] = 1;

    // checking all the cities have been visited 
    for(int i = 0; i < cityNum; i++){
        if (visited[i] != 0){
            count = count + 1;
        }
    } 

    visited[curCity] = count;

    // checking to see if all cities have been visited.
    // if all cities have been visted and the current cost is less than the best cost then update the best cost and best path

    if (count == cityNum){
        if (curCost < bestCost){
            bestCost = curCost;

            for(int i = 0; i < cityNum; i++){
                bestPath[i] = visited[i];
            }
        }
        return;
    }

    // if logging print the paths the have been dropped to the log file
    if (logging){
        if (curCost > bestCost){
            for(int i = 0; i < cityNum; i++){
                if (visited[i < count]){
                    fprintf(log_file, "%d ", visited[i] + 1);
                }
            }
            fprintf(log_file, ": dropped path because %d > %d", curCost, bestCost);
            return;
        }
    }

    // loop through the visted array. If we have visted the city before or we are currently at the city do nothing 
    // else update the final cost with current cost and the cost at the current city
    // recursevly call tsp function to check all cities 
    for(int i = 0; i < cityNum; i++){
        if(visited[i] != 0 || i == curCity){

        }
        else {
            int finalCost = curCost + getMatrix(curCity,i); 
            tsp(i, visited, finalCost);
        }
    }
}

int main(int argc, char **argv){
    int comm_sz; // Number of processes
    int my_rank; // My process rank

    MPI_Init(NULL, NULL); // Start up MPI
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); // Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // Get my rank among all the processes 

    FILE * input_file;
    clock_t start_t, end_t, total_t;

    //starting the clock
    start_t = clock();

    //initalise the distance matrix in the master (process 0)
    if (my_rank == 0){
        for (int i = 0; i < comm_sz; i++){
            // defining the file name and assigning the file name to be == to the first argument
            input_file = fopen(argv[1], "r");

            // checking if a log file needs to be created
            // if we need to create a log file. Create the log file and open the text file
            if(argc >= 2){ 
                logging = 1;
                log_file = fopen("log.txt","w+");
                if(log_file == NULL) {
                    printf("error");
                } 
            }

            // read the first line to figure out how may cities are being visited 
            fscanf(input_file, "%s", buff);
            cityNum = atoi(buff); 

            // find the size of the matrix based of the first input
            matrixSize = cityNum * cityNum;

            // delcare integer to track what line of the file we are currently on
            int trackLines = 1;

            // read through the input file row by column and create the distance matrix
            for (int i = 1; i < cityNum; i++){
                for (int j = 0; j < trackLines; j++){
                    fscanf(input_file, "%s", buff); 
                    int value = atoi(buff);

                    setMatrix(i,j, value); 
                    setMatrix(j,i, value); 
                }
            trackLines++;
            }
        }
    }

    if (my_rank == 0){
        for (int i = 0; i < comm_sz; i++){
            //assign the different tsp branches to each of the other processors  
            MPI_Send(distanceMatrix, (cityNum - 1) * (cityNum - 1), MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    else{
        for (int i = 0; i < comm_sz-1; i++){
            // using the matrix size allocate the appropriate amount of space for the distance matrix 
            distanceMatrix = allocateMatrix(matrixSize);
            MPI_Recv(distanceMatrix, matrixSize, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // create a array that check if each city has been visited
            int visited[cityNum];   
            for (int i = 0; i < cityNum; i++) {
                visited[i] = 0;
            }

            // call the tsp function, passing 0 through as the current cost and city
            for(int k = 0; k < comm_sz; k++){
                tsp(curCity,visited,curCost);
            }
        }
    }
    
    // loop through the best path array and print this array
    printf("Fastest path: ");
    for (int i = 0; i < cityNum; i++) {
        if (i < cityNum - 1) { 
            printf("%d,", bestPath[i]);
        }
        else {
            printf("%d", bestPath[i]);
        }
    }

    // print the best cost
    printf("\nDistance: %d\n", bestCost);

    // close the connection
    fclose(input_file);

    if (argc>2) {
        while (!feof(log_file)) {
            fgets(buff,10,log_file);
        }
        fclose(log_file);
    }

    if (logging){
        fclose(log_file);
    }

    // stop the clock and calculate the time 
    end_t = clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
    MPI_Finalize();
}