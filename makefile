
hello: hello.c hello.cl
	gcc -o hello hello.c -lOpenCL

# Mac
#hello: hello.c hello.cl
#	gcc -o hello hello.c -framework opencl
