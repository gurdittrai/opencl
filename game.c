#define PROGRAM_FILE "game.cl"
#define KERNEL_FUNC "game"
// 24x24 board
#define ROW_SIZE 4
#define ARRAY_SIZE 16

#define RAND 0

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <unistd.h>

// can request deprecated version of OpenCL API using:
//   #define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 220

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "opencl_struct.h"

int *initBoard(int test)
{
    int i, j;

    int *board = malloc(sizeof(int) * ARRAY_SIZE);

    for (i = 0; i < ARRAY_SIZE; i += ROW_SIZE)
    {
        // assign values
        for (j = 0; j < ROW_SIZE; j += 1)
        {
            //random
            // int r = ((random() % 100) > 45) ? 0 : 1;
            // board[i + j] = r * RAND;
            board[i + j] = test * (i + j);
        }
    }

    return board;
}

void printBoard(int *board)
{
    int i, j;
    for (i = 0; i < ARRAY_SIZE; i += ROW_SIZE)
    {
        for (j = 0; j < ROW_SIZE; j += 1)
        {
            printf("%3d ", board[i + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char **argv)
{

    int SIZE = 1024;

    // Allocate memories for input arrays and output array.
    float *A = (float *)malloc(sizeof(float) * SIZE);
    float *B = (float *)malloc(sizeof(float) * SIZE);

    // Output
    float *C = (float *)malloc(sizeof(float) * SIZE);

    // Initialize values for array members.
    int i = 0;
    for (i = 0; i < SIZE; ++i)
    {
        A[i] = i + 1;
        B[i] = (i + 1) * 2;
    }

    // Load kernel from file vecAddKernel.cl

    FILE *kernelFile;
    char *kernelSource;
    size_t kernelSize;

    kernelFile = fopen("vecAddKernel.cl", "r");

    if (!kernelFile)
    {

        fprintf(stderr, "No file named vecAddKernel.cl was found\n");

        exit(-1);
    }
    // kernelSource = (char *)malloc(MAX_SOURCE_SIZE);
    // kernelSize = fread(kernelSource, 1, MAX_SOURCE_SIZE, kernelFile);
    // fclose(kernelFile);

    // Getting platform and device information
    cl_platform_id platformId = NULL;
    cl_device_id deviceID = NULL;
    cl_uint retNumDevices;
    cl_uint retNumPlatforms;
    cl_int ret = clGetPlatformIDs(1, &platformId, &retNumPlatforms);
    ret = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_DEFAULT, 1, &deviceID, &retNumDevices);

    // Creating context.
    cl_context context = clCreateContext(NULL, 1, &deviceID, NULL, NULL, &ret);

    // Creating command queue
    cl_command_queue commandQueue = clCreateCommandQueueWithProperties(context, deviceID, 0, &ret);

    // Memory buffers for each array
    cl_mem aMemObj = clCreateBuffer(context, CL_MEM_READ_ONLY, SIZE * sizeof(float), NULL, &ret);
    cl_mem bMemObj = clCreateBuffer(context, CL_MEM_READ_ONLY, SIZE * sizeof(float), NULL, &ret);
    cl_mem cMemObj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, SIZE * sizeof(float), NULL, &ret);

    // Copy lists to memory buffers
    ret = clEnqueueWriteBuffer(commandQueue, aMemObj, CL_TRUE, 0, SIZE * sizeof(float), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(commandQueue, bMemObj, CL_TRUE, 0, SIZE * sizeof(float), B, 0, NULL, NULL);

    // create program
    cl_program program = build_program(context, deviceID, PROGRAM_FILE);

    // Create program from kernel source
    //cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, (const size_t *)&kernelSize, &ret);

    // Build program
    ret = clBuildProgram(program, 1, &deviceID, NULL, NULL, NULL);

    // Create kernel
    cl_kernel kernel = clCreateKernel(program, KERNEL_FUNC, &ret);

    // Set arguments for kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&aMemObj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bMemObj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&cMemObj);

    // Execute the kernel
    size_t globalItemSize = SIZE;
    size_t localItemSize = 64; // globalItemSize has to be a multiple of localItemSize. 1024/64 = 16
    ret = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &globalItemSize, &localItemSize, 0, NULL, NULL);

    // Read from device back to host.
    ret = clEnqueueReadBuffer(commandQueue, cMemObj, CL_TRUE, 0, SIZE * sizeof(float), C, 0, NULL, NULL);

    // Write result
    /*
	for (i=0; i<SIZE; ++i) {

		printf("%f + %f = %f\n", A[i], B[i], C[i]);

	}
	*/

    // Test if correct answer
    for (i = 0; i < SIZE; ++i)
    {
        if (C[i] != (A[i] + B[i]))
        {
            printf("Something didn't work correctly! Failed test. \n");
            break;
        }
    }
    if (i == SIZE)
    {
        printf("Everything seems to work fine! \n");
    }

    // Clean up, release memory.
    ret = clFlush(commandQueue);
    ret = clFinish(commandQueue);
    ret = clReleaseCommandQueue(commandQueue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(aMemObj);
    ret = clReleaseMemObject(bMemObj);
    ret = clReleaseMemObject(cMemObj);
    ret = clReleaseContext(context);
    free(A);
    free(B);
    free(C);

    return 0;
}