#ifndef OCL_STRUCTURES_H
#define OCL_STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>

// can request deprecated version of OpenCL API using:
//   #define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 220

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id device, const char *filename)
{

    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    int err;

    /* Read program file and place content into buffer */
    // read file
    program_handle = fopen(filename, "r");
    if (program_handle == NULL)
    {
        perror("Couldn't find the program file");
        exit(1);
    }

    // check size
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);

    // reset
    rewind(program_handle);

    // allocate space and place end
    program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';

    // place program into buffer
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    /* Create program from file */
    program = clCreateProgramWithSource(ctx, 1,
                                        (const char **)&program_buffer, &program_size, &err);
    if (err < 0)
    {
        perror("Couldn't create the program");
        exit(1);
    }
    free(program_buffer);

    /* Build program */
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err < 0)
    {
        perror("Couldn't build the program");

        /* Find size of log and print to std output */
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 
                0, NULL, &log_size);
        program_log = (char*) malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 
                log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
        exit(1);
    }

    return program;
}

#endif
