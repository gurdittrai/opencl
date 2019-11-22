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

    /* Get Platform and Device Info */
    cl_device_id device = NULL;
    device = create_device();

    /* Create OpenCL context */
    cl_context context = NULL;
    context = create_context(device);

    /* Build program */
    cl_program program;
    program = build_program(context, device, PROGRAM_FILE);

    /* Create data buffer */
    cl_int err;
    size_t global_size = ARRAY_SIZE;
    // size_t local_size = 4;
    cl_mem board_1_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, ARRAY_SIZE * sizeof(int), board_1, &err);
    cl_mem board_2_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, ARRAY_SIZE * sizeof(int), board_2, &err);
    if (err < 0)
    {
        perror("Couldn't create a buffer");
        exit(1);
    }

    /* Create Command Queue */
    cl_command_queue queue = NULL;
    queue = create_command_queue(context, device);

    /* Create a kernel */
    cl_kernel kernel = NULL;
    kernel = create_kernel(program, KERNEL_FUNC);

    /* Create kernel arguments */
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &board_1_buffer);
    err |= clSetKernelArg(kernel, 1, ARRAY_SIZE * sizeof(int), NULL);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &board_2_buffer);
    err |= clSetKernelArg(kernel, 3, ARRAY_SIZE * sizeof(int), NULL);
    if (err < 0)
    {
        perror("Couldn't create a kernel argument");
        exit(1);
    }

    /* Enqueue kernel */
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size,
                                 NULL, 0, NULL, NULL);
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