#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

void check_err(cl_int err, char* err_output) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error %i: %s\n", (int)err, err_output);
        exit(1);
    }
}

int * allocate(int size) {
    return calloc(size, sizeof(int));
}

int main(int argc, char **argv) {
    char *fileName; 
    FILE * input_file;

    // setting the filename
    if (argc >= 2) {
        fileName = argv[1];
    }

    else if (argc != 2 && argc != 3) {
        printf("Invalid Arguments");
        return 0;
    }

    // defining the file name and assigning the file name to be == to the first argument
    input_file = fopen(fileName, "r");

    // checking input file is working
    if (input_file == NULL)
    {
        printf("Error! Could not open file: %s\n", fileName);
        return 0;
    }

    char buff[255];
    fgets(buff, 255, (FILE *)input_file);
    int size = atoi(buff);
    const int LIST_SIZE = size;

    // using the matrix size allocate the appropriate amount of space for array 
    int * arr = allocate(size);

    // read through the input file row by column and create the array
    for (int i = 0; i < size; i++){
        fscanf(input_file, "%s", buff); 
        arr[i] = atoi(buff);
    }

    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("kernel.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    // variable used for OpenCL error handling
    cl_int err;

    // Get the platform ID
    cl_platform_id platform_id;
    cl_uint ret_num_platforms;
    err = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    check_err(err, "clGetPlatformIDs");
    printf("%i Platforms found\n", ret_num_platforms);

    // Get the first GPU device associated with the platform
    cl_device_id device_id;
    cl_uint ret_num_devices;
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
    check_err(err, "clGetPlatformIDs");
    printf("%i Devices found\n", ret_num_devices);

    // Get maximum work group size
    size_t max_work_group_size;
    err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, NULL);
    check_err(err, "clGetDeviceInfo");
    printf("Max workgroup size is %i\n", (int)max_work_group_size);

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    check_err(err, "clCreateContext");

    // Create Queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &err);
    check_err(err, "clCreateCommandQueue");

    // Create memory buffers on the device for each vector
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, LIST_SIZE * sizeof(int), NULL, &err);
    check_err(err, "clCreateBuffer a");
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int), NULL, &err);
    check_err(err, "clCreateBuffer b");

    // Copy the lists A and B to their respective memory buffers
    err = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), arr, 0, NULL, NULL);
    check_err(err, "clEnqueueWriteBuffer a");
    err = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, sizeof(int), &size, 0, NULL, NULL);
    check_err(err, "clEnqueueWriteBuffer b");

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &err);
    check_err(err, "clCreateProgramWithSource");

    // Build the program
    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    // Determine the size of the log
    size_t log_size;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    // Allocate memory for the log
    char *log = (char *) malloc(log_size);
    // Get the log
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
    // Print the log
    printf("%s\n", log);
    check_err(err, "clBuildProgram");

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "vector_add", &err);
    check_err(err, "clCreateKernel");

    // Set the arguments of the kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    check_err(err, "clSetKernelArg a");
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    check_err(err, "clSetKernelArg b");

    // Launch Kernel linked to an event
    size_t local_work_size = ((size_t)LIST_SIZE > max_work_group_size)? max_work_group_size : (size_t)LIST_SIZE;
    size_t num_work_groups = (LIST_SIZE + local_work_size - 1) / local_work_size; // divide rounding up
    size_t global_work_size = num_work_groups * local_work_size;
    err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
    check_err(err, "clEnqueueNDRangeKernel");

    // Read the memory buffer C on the device to the local variable C
    err = clEnqueueReadBuffer(command_queue, a_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), arr, 0, NULL, NULL);
    check_err(err, "clEnqueueReadBuffer c");

    // Display the result to the screen
    for (int i = 0; i < LIST_SIZE; i++){
        printf("%d ",arr[i]);
    }

    // Clean up
    err = clFlush(command_queue);
    err = clFinish(command_queue);
    err = clReleaseKernel(kernel);
    err = clReleaseProgram(program);
    err = clReleaseMemObject(a_mem_obj);
    err = clReleaseMemObject(b_mem_obj);
    err = clReleaseCommandQueue(command_queue);
    err = clReleaseContext(context);
    return 0;
}
