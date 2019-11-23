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

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device()
{

    cl_platform_id platform = NULL;
    cl_device_id device_id = NULL;
    cl_int err;

    /* Identify a platform */
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err < 0)
    {
        perror("Couldn't identify a platform");
        exit(1);
    }

    /* Access a device */
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, NULL);
    if (err < 0)
    {
        perror("Couldn't access any devices");
        exit(1);
    }

    return device_id;
}

cl_context create_context(cl_device_id device)
{
    cl_context context;
    cl_int err;
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err < 0)
    {
        perror("Couldn't create a context");
        exit(1);
    }
    return context;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id device, const char *filename)
{

    cl_program program;
    FILE *program_handle;
    char *program_buffer;
    size_t program_size;
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
        exit(1);
    }

    return program;
}

cl_kernel create_kernel(cl_program program, const char* kernel_func)
{
    cl_kernel kernel;
    cl_int err;
    kernel = clCreateKernel(program, kernel_func, &err);
    if (err < 0)
    {
        perror("Couldn't create a kernel");
        exit(1);
    }
    return kernel;
}

cl_command_queue create_command_queue(cl_context context, cl_device_id device)
{
    cl_command_queue queue;
    cl_int err;
    queue = clCreateCommandQueueWithProperties(context, device, 0, &err);
    if (err < 0)
    {
        perror("Couldn't create a command queue");
        exit(1);
    }
    return queue;
}



