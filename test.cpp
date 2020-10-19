#include <iostream>
#ifdef __APPLE__
    #include <OpenCL/cl.hpp>
#else
    #include <CL/cl.hpp>
#endif
int main(){
	// get all platforms (drivers), e.g. NVIDIA
	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);
	if (all_platforms.size()==0) {
		std::cout<<" No platforms found. Check OpenCL installation!\n";
		exit(1);
	}
	cl::Platform default_platform=all_platforms[0];
	std::cout << "Using platform: "<<default_platform.getInfo<CL_PLATFORM_NAME>()<<"\n";

	// get default device (CPUs, GPUs) of the default platform
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	if(all_devices.size()==0){
		std::cout<<" No devices found. Check OpenCL installation!\n";
		exit(1);
	}
	// use device[1] because that's a GPU; device[0] is the CPU
	cl::Device default_device=all_devices[1];
	std::cout<< "Using device: "<<default_device.getInfo<CL_DEVICE_NAME>()<<"\n";
	return 0;
}
