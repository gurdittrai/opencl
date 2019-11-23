#define PROGRAM_FILE "game.cl"
#define KERNEL_FUNC "game"
// 24x24 board (576 elements)
#define ROW_SIZE 6
#define ARRAY_SIZE 36

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

void insert_shapes(int *board)
{
    int ofs = ROW_SIZE;
    // check
    board[(0 * ofs) + 0] = 1;
    board[(0 * ofs) + 5] = 0;
    board[(5 * ofs) + 0] = 0;
    board[(5 * ofs) + 5] = 1;

    // toad
    board[(2 * ofs) + 2] = 1;
    board[(2 * ofs) + 3] = 1;
    board[(2 * ofs) + 4] = 1;
    board[(3 * ofs) + 1] = 1;
    board[(3 * ofs) + 2] = 1;
    board[(3 * ofs) + 3] = 1;
}

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
            // board[i + j] = test * (i + j);
            board[i + j] = ((i + j + test) % 2) * RAND;
        }
    }

    if (ARRAY_SIZE == 36)
    {
        insert_shapes(board);
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
    // initiaze variables
    int bytes = ARRAY_SIZE * sizeof(int);
    int k_cnt = 2;
    int output = 1;
    int i, turnA = 0, turnB = 1;
    size_t global_size = ARRAY_SIZE;
    size_t local_size = ROW_SIZE;

    // boards A and B
    int **board = malloc(sizeof(int*) * 2);
    board[0] = initBoard(1);
    board[1] = malloc(sizeof(int) * ARRAY_SIZE);

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
    cl_mem buffer[2];
    buffer[0] = clCreateBuffer(context, CL_MEM_READ_WRITE, bytes, NULL, &ret);
    buffer[1] = clCreateBuffer(context, CL_MEM_READ_WRITE, bytes, NULL, &ret);

    // Copy lists to memory buffers
    ret = clEnqueueWriteBuffer(commandQueue, buffer[0], CL_TRUE, 0, bytes, board[0], 0, NULL, NULL);

    // create program
    cl_program program = build_program(context, deviceID, PROGRAM_FILE);

    // Build program
    ret = clBuildProgram(program, 1, &deviceID, NULL, NULL, NULL);

    // Create kernel
    cl_kernel kernel[k_cnt];
    for (i = 0; i < k_cnt; i += 1)
        kernel[i] = clCreateKernel(program, KERNEL_FUNC, &ret);

    // Set arguments for kernel
    for (i = 0; i < k_cnt; i += 1)
    {
        ret = clSetKernelArg(kernel[i], 0, sizeof(cl_mem), (void *)&buffer[turnA]);
        ret = clSetKernelArg(kernel[i], 1, sizeof(cl_mem), (void *)&buffer[turnB]);

        // swap buffers
        int temp = turnA;
        turnA = turnB;
        turnB = temp;
    }

    turnA = 0;
    turnB = 1;
    for (i = 0; i < 10; i += 1)
    {
        // Execute the kernel
        ret = clEnqueueNDRangeKernel(commandQueue, kernel[turnA], 1, NULL, &global_size, &local_size, 0, NULL, NULL);

        // Read from device back to host.
        ret = clEnqueueReadBuffer(commandQueue, buffer[turnB], CL_TRUE, 0, bytes, board[turnB], 0, NULL, NULL);

        printBoard(board[turnA]);
        printBoard(board[turnB]);
        printf("--\n");

        // swap buffers
        int temp = turnA;
        turnA = turnB;
        turnB = temp;

        // // Execute the kernel 2
        // ret = clEnqueueNDRangeKernel(commandQueue, kernel[1], 1, NULL, &global_size, &local_size, 0, NULL, NULL);

        // // Read from device back to host.
        // ret = clEnqueueReadBuffer(commandQueue, buffer[0], CL_TRUE, 0, bytes, A, 0, NULL, NULL);

        // printBoard(A);
        // printBoard(B);
        // printf("--\n");

    }

    // Write result
    if (output == 1)
        printf("Finished\n");

    // Clean up, release memory.
    ret = clFlush(commandQueue);
    ret = clFinish(commandQueue);
    ret = clReleaseCommandQueue(commandQueue);
    for (i = 0; i < k_cnt; i += 1)
        ret = clReleaseKernel(kernel[i]);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(buffer[0]);
    ret = clReleaseMemObject(buffer[1]);
    ret = clReleaseContext(context);
    free(board[0]);
    free(board[1]);
    free(board);

    return 0;
}