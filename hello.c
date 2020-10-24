#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
    #include <OpenCL/cl.h>
#else
	#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
	#include <CL/cl.h>
#endif
int main(){
	cl_uint platformCount;
	cl_platform_id* platforms;
	cl_device_id* devices;	
	cl_uint num_devices;
	cl_int ciErrNum;
	size_t global;                      // global domain size for our calculation
    	size_t local;                       // local domain size for our calculation
	size_t mem_size = 5;
	int src_image[] = {1,2,3,4,5};
	int results[5];

	const char *source = 	"   void kernel simple_add(global const int* A, global int* C){"
				"       int ID, Nthreads, n, ratio, start, stop;"
				"       ID = get_global_id(0);"
				"       Nthreads = get_global_size(0);"
				"       n = 5;"
				"       ratio = (n / Nthreads);"  // number of elements for each thread
				"       start = ratio * ID;"   
				"       stop  = ratio * (ID + 1);"
				"       for (int i=start; i< n; i++)"
				"           C[i] = A[i];"
				"   }";
	size_t program_length = strlen(source);
	




	//get all platforms
	clGetPlatformIDs(0, NULL, &platformCount);
	platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
	clGetPlatformIDs(platformCount, platforms, NULL);

	if(platformCount > 0){
		printf("Number of platform is %d\n", platformCount);
	}
	



	//get all devices
	clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
	devices = (cl_device_id*) malloc(sizeof(cl_device_id) * num_devices);

	int err;
	err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);

	if(num_devices > 0){
		printf("Number of devices is %d\n", num_devices);
		if ( err != CL_SUCCESS){
			printf("Error: Failed to create a device group!\n");
			exit(EXIT_FAILURE);
		}
	}
	size_t valueSize;
	clGetDeviceInfo(devices[0], CL_DEVICE_NAME, 0, NULL, &valueSize);
	char* value = (char*) malloc(valueSize);
	clGetDeviceInfo(devices[0], CL_DEVICE_NAME, valueSize, value, NULL);
	printf("Device: %s\n",value);
	free(value);





	// Create context
	cl_context myctx = clCreateContext(0, 1,devices, NULL, NULL, &ciErrNum);
	if(!myctx){
		printf("Error: Failed to create a compute context!\n");
        	exit(EXIT_FAILURE);
	}



	// Create queue commands
	cl_command_queue myqueue;
	myqueue = clCreateCommandQueue(myctx, devices[0], 0, &ciErrNum);
	if (!myqueue){
		printf("Error: Failed to create a command commands!\n");
		exit(EXIT_FAILURE);
	}	


	// Build program	
	cl_program myprog = clCreateProgramWithSource( myctx,1, (const char **)&source, NULL, &ciErrNum);
	//cl_program myprog = clCreateProgramWithSource( myctx,1, (const char **)&source_1, NULL, &ciErrNum);
	if (!myprog){
		printf("Error: Failed to create compute program!\n");
		exit(EXIT_FAILURE);
	}
	// build the program
	ciErrNum = clBuildProgram( myprog, 0, NULL, NULL, NULL, NULL);
	if(ciErrNum != CL_SUCCESS){
		size_t len;
		char buffer[2048];
		printf("Error: Failed to build program executable!\n");
		clGetProgramBuildInfo(myprog, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		printf("%s\n", buffer);
		exit(1);
	}




	//Use the “simple_add” function as the kernel
	cl_kernel mykernel = clCreateKernel(myprog ,"simple_add" ,&ciErrNum);

	if (!mykernel || ciErrNum != CL_SUCCESS){
		printf("Error: Failed to create compute kernel!\n");
		exit(EXIT_FAILURE);
	}




	

	// create buffers on DEVICE	
	// INPUT DATA
	cl_mem d_ip = clCreateBuffer(myctx, CL_MEM_READ_ONLY, mem_size*sizeof(int), NULL, &ciErrNum);
	// OUTPUT DATA IS WRITE ONLY	
	cl_mem d_op = clCreateBuffer(myctx, CL_MEM_WRITE_ONLY, mem_size*sizeof(int), NULL, &ciErrNum);
	if (!d_ip || !d_op || ciErrNum != CL_SUCCESS){
		printf("Error: Failed to allocate device memory!\n");
		exit(EXIT_FAILURE);
	}


	

	// Write our data set into the input array in device memory
	ciErrNum = clEnqueueWriteBuffer (myqueue , d_ip, CL_TRUE, 0, mem_size*sizeof(int), (void *)src_image, 0, NULL, NULL);
	if (ciErrNum != CL_SUCCESS){
		printf("Error: Failed to write to source array!\n");
		exit(EXIT_FAILURE);
	}


	


	// Set the arguments to our compute kernel
	ciErrNum = 0;
	ciErrNum = clSetKernelArg(mykernel, 0, sizeof(cl_mem), &d_ip);
	ciErrNum |= clSetKernelArg(mykernel, 1, sizeof(cl_mem), &d_op);
	if (ciErrNum != CL_SUCCESS){
		printf("Error: Failed to set kernel arguments! %d\n", err);
		exit(EXIT_FAILURE);
	}




	// Get the maximum work group size for executing the kernel on the device
	ciErrNum = clGetKernelWorkGroupInfo(mykernel, devices[0], CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
	if (ciErrNum != CL_SUCCESS){
		printf("Error: Failed to retrieve kernel work group info! %d\n", err);
		exit(EXIT_FAILURE);
	}



	

	// Execute the kernel over the entire range of our 1d input data set
	// using the maximum number of work group items for this device
	global = 5;
	ciErrNum = clEnqueueNDRangeKernel(myqueue, mykernel, 1, NULL, &global, &local, 0, NULL, NULL);
	if (ciErrNum != CL_SUCCESS){
		printf("Error: Failed to execute kernel!\n");
		exit(EXIT_FAILURE);
	}




	
	// Wait for the command commands to get serviced before reading back results
	// IMPORTANT COMMANDS
	clFinish(myqueue);





	
	// Read back the results from the device to verify the output
	ciErrNum = clEnqueueReadBuffer(myqueue, d_op, CL_TRUE, 0, sizeof(int) * mem_size, results, 0, NULL, NULL ); 
	if (ciErrNum != CL_SUCCESS){
		printf("Error: Failed to read output array! %d\n", ciErrNum);
		exit(EXIT_FAILURE);
	}


	// Validate our results
	printf("Results:\n");
	for(int i = 0; i < mem_size; i++){
		printf("%d ", results[i]);	
	}
	printf("\n");




	// Shutdown and cleanup
	clReleaseMemObject(d_ip);
	clReleaseMemObject(d_op);
	clReleaseProgram(myprog);
	clReleaseKernel(mykernel);
	clReleaseCommandQueue(myqueue);
	clReleaseContext(myctx);
	return 0;
}
