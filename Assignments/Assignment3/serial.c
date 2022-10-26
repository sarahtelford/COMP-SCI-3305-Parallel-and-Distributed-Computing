#include <stdio.h>
#include <stdlib.h>

int * allocate(int size) {
    return calloc(size, sizeof(int));
}

int main(int argc, char **argv){
    char * nameFile;
    FILE * input_file;
    
    // defining the file name and assigning the file name to be == to the first argument
    input_file = fopen(argv[1], "r");
    
    char buff[255];
    
    // read the first line to figure out how may cities are being visited
    fscanf(input_file, "%s", buff);
    int size = atoi(buff);
    
    
    // using the matrix size allocate the appropriate amount of sapce for the distance matrix
    int * arr = allocate(size);
    
    // read through the input file row by column and create the array
    for (int i = 0; i < size; i++){
        fscanf(input_file, "%s", buff);
        arr[i] = atoi(buff);
    }
    
    for (int j=0; j < size; j++){
        for (int i = 0; i < size; i++){
            if (i % 2 == 0) {
                if (arr[i] > arr[i+1]) {
                    if (i+1 != size){
                        int temp;
                        temp = arr[i];
                        arr[i] = arr[i+1];
                        arr[i+1] = temp;
                    }
                }
            }
            
            if (i % 2 == 1){
                if (arr[i] > arr[i+1]) {
                    if (i+1 != size){
                        int temp;
                        temp = arr[i];
                        arr[i] = arr[i+1];
                        arr[i+1] = temp;
                    }
                }
            }
        }
    }
    
    for (int i = 0; i < size; i++){
        printf("%d ",arr[i]);
    }
}
