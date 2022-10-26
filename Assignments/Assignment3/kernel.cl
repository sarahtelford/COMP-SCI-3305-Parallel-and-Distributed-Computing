__kernel void vector_add(__global int *A, __global int *B) {

    // Get the index of the current element to be processed
    int i = get_global_id(0);
 
    for (int j=0; j < B[0]; j++){
         if (i % 2 == 0) { 
            if (A[i] > A[i+1]) {
                if (i+1 != B[0]){ 
                    int temp;
                    temp = A[i];
                    A[i] = A[i+1];
                    A[i+1] = temp;
                }
            } 
        }
        barrier(CLK_GLOBAL_MEM_FENCE);

        if (i % 2 == 1){
            if (A[i] > A[i+1]) {
                if (i+1 != B[0]){ 
                    int temp;
                    temp = A[i];
                    A[i] = A[i+1];
                    A[i+1] = temp;
                }
            } 
        }
        barrier(CLK_GLOBAL_MEM_FENCE);
    }
}

