#define PROGRAM_FILE "game.cl"
#define KERNEL_FUNC "game_simulation"
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

int main()
{
    // board
    int *board_1;
    int *board_2;

    // initailize board
    board_1 = initBoard(1);
    board_2 = initBoard(0);

    printBoard(board_1);
    printBoard(board_2);

    // opencl

    /* Get Platform and Device Info */
    cl_device_id device = NULL;
    device = create_device();

    /* Create OpenCL context */
    cl_context context = NULL;
    context = create_context(device);

    /* Build program */
    cl_program program;
    program = build_program(context, device, PROGRAM_FILE);

    /* Create Command Queue */
    cl_command_queue queue = NULL;
    queue = create_command_queue(context, device);

    /* Create a kernel */
    cl_kernel kernel = NULL;
    kernel = create_kernel(program, KERNEL_FUNC);

    /* Create data buffer */
    cl_int err;
    cl_mem board_1_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, ARRAY_SIZE * sizeof(int), NULL, &err);
    cl_mem board_2_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, ARRAY_SIZE * sizeof(int), NULL, &err);
    if (err < 0)
    {
        perror("Couldn't create a buffer");
        exit(1);
    }

    /* Write to buffer */
    err = clEnqueueWriteBuffer(queue, board_1_buffer, CL_TRUE, 0, ARRAY_SIZE * sizeof(int), board_1, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, board_2_buffer, CL_TRUE, 0, ARRAY_SIZE * sizeof(int), board_2, 0, NULL, NULL);
    

    /* Create kernel arguments */
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &board_1_buffer);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &board_2_buffer);
    if (err < 0)
    {
        perror("Couldn't create a kernel argument");
        exit(1);
    }

    /* Enqueue kernel */
    size_t global_size = ARRAY_SIZE;
    size_t local_size = 8;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size,
                                 &local_size, 0, NULL, NULL);
    if (err < 0)
    {
        perror("Couldn't enqueue the kernel");
        exit(1);
    }

    /* Read the kernel's output */
    err = clEnqueueReadBuffer(queue, board_2_buffer, CL_TRUE, 1,
                              ARRAY_SIZE * sizeof(int), board_2, 0, NULL, NULL);
    if (err < 0)
    {
        perror("Couldn't read the buffer");
        exit(1);
    }

    printBoard(board_1);
    printBoard(board_2);

    /* Deallocate resources */
    clReleaseKernel(kernel);
    clReleaseMemObject(board_1_buffer);
    clReleaseMemObject(board_2_buffer);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    return 0;
}