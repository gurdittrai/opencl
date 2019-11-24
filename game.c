#define PROGRAM_FILE "game.cl"
#define KERNEL_FUNC "game"

// 24 x 24 board (576 elements)
#define ROW_SIZE 24
#define ARRAY_SIZE 576

#define RAND 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#include "board.h"
#include "ocl_structures.h"

void get_args(int argc, char *argv[], int *output, int *k_cnt)
{
    for (int i = 1; i < argc; i += 1)
    {
        if (strcmp(argv[i], "-o") == 0)
        {
            *output = 0;
        }
        else if (strcmp(argv[i], "-n") == 0)
        {
            // check count arg exists
            if (i + 1 > argc)
                continue;

            // get arg (1-6)
            *k_cnt = argv[i + 1][0] - '0';
            if ((*k_cnt < 0) || (*k_cnt > 6))
            {
                printf("invalid kernel count (default 2 used)\n");
                *k_cnt = 2;
            }
        }
    }
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
    int **board = malloc(sizeof(int *) * 2);
    board[0] = initBoard(ARRAY_SIZE, ROW_SIZE);
    board[1] = initBoard(ARRAY_SIZE, ROW_SIZE);

    // get args
    get_args(argc, argv, &output, &k_cnt);

    // getting platform and device information
    cl_platform_id platformId = NULL;
    cl_device_id deviceID = NULL;
    cl_int ret;
    ret = clGetPlatformIDs(1, &platformId, NULL);
    ret = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_DEFAULT, 1, &deviceID, NULL);

    // creating context
    cl_context context = clCreateContext(NULL, 1, &deviceID, NULL, NULL, &ret);

    // creating command queue
    cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, deviceID, 0, &ret);

    // buffers for each array
    cl_mem buffer[2];
    buffer[0] = clCreateBuffer(context, CL_MEM_READ_WRITE, bytes, NULL, &ret);
    buffer[1] = clCreateBuffer(context, CL_MEM_READ_WRITE, bytes, NULL, &ret);

    // initialize buffer
    ret = clEnqueueWriteBuffer(command_queue, buffer[0], CL_TRUE, 0, bytes, board[0], 0, NULL, NULL);

    // create program
    cl_program program = build_program(context, deviceID, PROGRAM_FILE);

    // create kernel array
    cl_kernel kernel[k_cnt];
    for (i = 0; i < k_cnt; i += 1)
        kernel[i] = clCreateKernel(program, KERNEL_FUNC, &ret);

    // arguments for kernel
    for (i = 0; i < k_cnt; i += 1)
    {
        ret = clSetKernelArg(kernel[i], 0, sizeof(cl_mem), (void *)&buffer[turnA]);
        ret = clSetKernelArg(kernel[i], 1, sizeof(cl_mem), (void *)&buffer[turnB]);

        // swap buffers
        int temp = turnA;
        turnA = turnB;
        turnB = temp;
    }

    int k_iter;
    turnA = 0;
    turnB = 1;

    // initialize curses
    if (output == 1)
    {
        initscr();
        noecho();
        cbreak();
        timeout(0);
        curs_set(FALSE);
    }

    for (i = 0, k_iter = 0; getch() != 'q' && i < 100; i += 1, k_iter += 1)
    {
        // reset iter
        if (k_iter >= k_cnt)
        {
            k_iter = 0;
        }

        // run kernel
        ret = clEnqueueNDRangeKernel(command_queue, kernel[k_iter], 1, NULL, &global_size, &local_size, 0, NULL, NULL);

        // get output
        ret = clEnqueueReadBuffer(command_queue, buffer[turnB], CL_TRUE, 0, bytes, board[turnB], 0, NULL, NULL);

        // ** uncomment to check output without ncurses **
        // printBoard(board[turnB], ARRAY_SIZE, ROW_SIZE);

        if (output == 1)
            drawBalls(board[turnB], ROW_SIZE, k_iter, turnB);

        // swap buffers
        int temp = turnA;
        turnA = turnB;
        turnB = temp;
    }

    // Write result
    if (output == 1)
    {
        endwin();
    }

    printf("successful exit after %d iterations\n", i);

    // free memory
    clFlush(command_queue);
    clFinish(command_queue);
    clReleaseCommandQueue(command_queue);

    for (i = 0; i < k_cnt; i += 1)
        clReleaseKernel(kernel[i]);
    
    clReleaseMemObject(buffer[0]);
    clReleaseMemObject(buffer[1]);
    clReleaseProgram(program);
    clReleaseContext(context);
    free(board[0]);
    free(board[1]);
    free(board);

    return 0;
}