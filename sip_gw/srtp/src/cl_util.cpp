#include "cl_util.h"
#include "utils.h"
#include "aes.h"

#include <math.h>

#define POOL_SIZE 30
//#define DEBUG


using namespace std;


// variables, settings & other stuff
static cl_platform_id platform;
static int dev_num = 1;
static cl_device_id devices[1];
static cl_uint dev_count;
static cl_uint dev_limit = 1;
static cl_context context;
static cl_command_queue cl_queue;
static cl_program program;
static cl_kernel inv_dct_kernel;
static cl_int error;
 
// buffers for dct
static cl_mem payload_src[POOL_SIZE];
static cl_mem payload_dst[POOL_SIZE];
static cl_mem iv_gpu[POOL_SIZE];
static cl_mem rk[POOL_SIZE];
static cl_mem t1[POOL_SIZE];
static cl_mem t2[POOL_SIZE];
 
// kernels
static cl_kernel encode_kernel;
static cl_kernel decode_kernel;


std::queue<int> free_buffers;

//max number of local work items
size_t GWS[3] = {16};
size_t LWS[3];

cl_int getPlatformID()
{
    cl_uint num_platforms;
    cl_platform_id *platform_ids;

    // Get OpenCL platform count
    cl_int error = clGetPlatformIDs (0, NULL, &num_platforms);
    checkClError(error, "platformID count");
    if(num_platforms == 0) {
        cout << "ERROR: 0 platforms found\n";
        return CL_OUT_OF_RESOURCES;
    }
    // if there's a platform or more, make space for ID's
    if((platform_ids = (cl_platform_id*)malloc(num_platforms*sizeof(cl_platform_id))) == NULL) {
        cout << "Failed to allocate memory for cl_platform ID's!\n";
        return CL_OUT_OF_RESOURCES;
    }

    // get platform info for each platform and trap the NVIDIA platform if found
    error = clGetPlatformIDs (num_platforms, platform_ids, NULL);
    checkClError(error, "platformIDs");

    //assign platform id to global variable
    platform = platform_ids[0];

    //clean resources
    free(platform_ids);

    return CL_SUCCESS;
}


size_t set_size(size_t i){
    /*if(i>=8){
        return 8;
    }
    else if(i<8 && i>=4){
        return 4;
    }
    else if(i<4 && i>=2){
        return 2;
    }
    else return 1;*/
    if(i<16){
        return i;
    }
    return 16;
}

void set_max_device_worksize(){
    error = clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_ITEM_SIZES,
            sizeof(LWS), LWS, NULL);

    size_t size;
    error = clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_GROUP_SIZE,
            sizeof(size), &size, NULL);

    LWS[0] = set_size(LWS[0]);
    LWS[1] = 1;
    LWS[2] = 1;
    checkClError(error, "clGetDeviceInfo");
}


int GPU::alloc_buffer(){
    while(free_buffers.empty());

    int id = free_buffers.front(); 
    free_buffers.pop();
    return id; 
}

void GPU::release_buffer(int id){
    free_buffers.push(id);
}

int initOpenCL(){
    error = getPlatformID();
    checkClError(error, "getPlatformID");

    // Device
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, dev_limit, devices, &dev_count);
    checkClError(error, "clGetDeviceIDs");

    // Context
    cl_context_properties contextProperties[] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0
    };
    context = clCreateContext(contextProperties, dev_num, devices,
                                        &contextCallback, NULL, &error);
    
    checkClError(error, "clCreateContext");

    //sets max number of workgroups
    set_max_device_worksize();

    // Command-queue
    cl_queue = clCreateCommandQueue(context, devices[0], 0, &error);
    checkClError(error, "clCreateCommandQueue");

    // Load kernels
    error = loadKernelFromFile("../src/srtp.cl", &decode_kernel, "srtp_decode");
    //error = loadKernelFromFile("../src/srtp.cl", &decode_kernel, "test");
    checkClError(error, "loadKernelFromFile srtp_decode");
    /*error = loadKernelFromFile("../src/srtp.cl", &encode_kernel, "srtp_encode");
    checkClError(error, "loadKernelFromFile srtp_encode");*/
  

    int length = 320;
    cl_int err;
    
    for(int i = 0; i<POOL_SIZE; i++){
        free_buffers.push(i);
        payload_src[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar)*length,
                    NULL, &err);
        checkClError(err, "clCreateBuffer src");
        
        payload_dst[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar)*length, 
                    NULL, &err);
        checkClError(err, "clCreateBuffer dst");

        iv_gpu[i] = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_uchar)*16, NULL, &err);
        checkClError(err, "clCreateBuffer iv");
        
        rk[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar)*16*11, NULL, &err);
        checkClError(err, "clCreateBuffer rk");
        
        t1[i] = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_uchar)*16, NULL, &err);
        checkClError(err, "clCreateBuffer t1");
        t2[i] = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_uchar)*16, NULL, &err);
        checkClError(err, "clCreateBuffer t2");
    }
    
}

cl_int loadKernelFromFile(const char* fileName, cl_kernel* kernel, char* kernel_name){
    ifstream src_file(fileName);
    if(!src_file.is_open()) return EXIT_FAILURE;

    string src_prog(istreambuf_iterator<char>(src_file), (istreambuf_iterator<char>()));
    const char *src = src_prog.c_str();
    size_t length = src_prog.length();

    program = clCreateProgramWithSource(context, 1, &src, &length, &error);
    checkClError(error, "clCreatePogramWithSource");

    error = clBuildProgram(program, dev_count, devices, NULL, NULL, NULL);
    checkClError(error, kernel_name);
    
    (*kernel) = clCreateKernel(program, kernel_name, &error);
    checkClError(error, "clCreateKernel");

    return CL_SUCCESS;
}


void srtp_decode_gpu(BYTE* src, BYTE* dst, BYTE* key, BYTE* iv, int length){ 
    // Alloc buffers with specified size & copy data
    cl_int err;

    size_t packet_size = sizeof(cl_uchar)*length;
  
    BYTE round_key[11][16];
    AES::expand_key(key, round_key);
   
    int index = GPU::alloc_buffer();

    // Copy data from memory to gpu
    err = clEnqueueWriteBuffer(cl_queue,
            payload_src[index], //memory on gpu
            CL_TRUE,   //blocking write
            0,         //offset
            sizeof(cl_uchar)*length, //size in bytes of copied data
            src,       //memory data
            0,         //wait list
            NULL,      //wait list
            NULL);     //wait list

    err = clEnqueueWriteBuffer(cl_queue,
            iv_gpu[index], //memory on gpu
            CL_TRUE,   //blocking write
            0,         //offset
            sizeof(cl_uchar)*16, //size in bytes of copied data
            iv,       //memory data
            0,         //wait list
            NULL,      //wait list
            NULL);     //wait list

    err = clEnqueueWriteBuffer(cl_queue,
            rk[index], //memory on gpu
            CL_TRUE,   //blocking write
            0,         //offset
            sizeof(cl_uchar)*16*11,
            round_key, //memory data
            0,         //wait list
            NULL,      //wait list
            NULL);     //wait list


    clSetKernelArg(decode_kernel, 0, sizeof(cl_mem), (void *)&(payload_src[index]));
    clSetKernelArg(decode_kernel, 1, sizeof(cl_mem), (void *)&(payload_dst[index]));
    clSetKernelArg(decode_kernel, 2, sizeof(cl_mem), (void *)&(iv_gpu[index]));
    clSetKernelArg(decode_kernel, 3, sizeof(int),    (void *)&length);
    clSetKernelArg(decode_kernel, 4, sizeof(cl_mem), (void *)&(rk[index]));
    clSetKernelArg(decode_kernel, 5, sizeof(cl_mem), (void *)&(t1[index]));
    clSetKernelArg(decode_kernel, 6, sizeof(cl_mem), (void *)&(t2[index]));

    clEnqueueNDRangeKernel(cl_queue, decode_kernel, 1, NULL, GWS, LWS, 0, NULL, NULL);
    
    // copy result back from gpu to host
    clEnqueueReadBuffer(cl_queue, 
            payload_dst[index], 
            CL_TRUE, 
            0, 
            sizeof(cl_uchar)*16,            
            dst, 
            0, 
            NULL, 
            NULL);

    clFinish(cl_queue);

    GPU::release_buffer(index);
    //err = clReleaseMemObject(payload_src);
    //checkClError(err, "clReleaseMemObject block_src");

    //err = clReleaseMemObject(payload_dst);
    //checkClError(err, "clReleaseMemObject block_dst");

    //err = clReleaseMemObject(key_gpu);
    //checkClError(err, "clReleaseMemObject key");

    //err = clReleaseMemObject(iv_gpu);
    //checkClError(err, "clReleaseMemObject iv");
    
    //err = clReleaseMemObject(rk);
    //checkClError(err, "clReleaseMemObject rk");

    //err = clReleaseMemObject(t1);
    //checkClError(err, "clReleaseMemObject t1");
    
    //err = clReleaseMemObject(t2);
    //checkClError(err, "clReleaseMemObject t2");
}


void srtp_encode_gpu(BYTE* src, BYTE* dst, BYTE* key, BYTE* iv, int length){
}

/**
 * Returns string for OpenCL error
 */
const char *CLErrorString(cl_int _err) {
    switch (_err) {
    case CL_SUCCESS:
        return "Success!";
    case CL_DEVICE_NOT_FOUND:
        return "Device not found.";
    case CL_DEVICE_NOT_AVAILABLE:
        return "Device not available";
    case CL_COMPILER_NOT_AVAILABLE:
        return "Compiler not available";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        return "Memory object allocation failure";
    case CL_OUT_OF_RESOURCES:
        return "Out of resources";
    case CL_OUT_OF_HOST_MEMORY:
        return "Out of host memory";
    case CL_PROFILING_INFO_NOT_AVAILABLE:
        return "Profiling information not available";
    case CL_MEM_COPY_OVERLAP:
        return "Memory copy overlap";
    case CL_IMAGE_FORMAT_MISMATCH:
        return "Image format mismatch";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        return "Image format not supported";
    case CL_BUILD_PROGRAM_FAILURE:
        return "Program build failure";
    case CL_MAP_FAILURE:
        return "Map failure";
    case CL_INVALID_VALUE:
        return "Invalid value";
    case CL_INVALID_DEVICE_TYPE:
        return "Invalid device type";
    case CL_INVALID_PLATFORM:
        return "Invalid platform";
    case CL_INVALID_DEVICE:
        return "Invalid device";
    case CL_INVALID_CONTEXT:
        return "Invalid context";
    case CL_INVALID_QUEUE_PROPERTIES:
        return "Invalid queue properties";
    case CL_INVALID_COMMAND_QUEUE:
        return "Invalid command queue";
    case CL_INVALID_HOST_PTR:
        return "Invalid host pointer";
    case CL_INVALID_MEM_OBJECT:
        return "Invalid memory object";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        return "Invalid image format descriptor";
    case CL_INVALID_IMAGE_SIZE:
        return "Invalid image size";
    case CL_INVALID_SAMPLER:
        return "Invalid sampler";
    case CL_INVALID_BINARY:
        return "Invalid binary";
    case CL_INVALID_BUILD_OPTIONS:
        return "Invalid build options";
    case CL_INVALID_PROGRAM:
        return "Invalid program";
    case CL_INVALID_PROGRAM_EXECUTABLE:
        return "Invalid program executable";
    case CL_INVALID_KERNEL_NAME:
        return "Invalid kernel name";
    case CL_INVALID_KERNEL_DEFINITION:
        return "Invalid kernel definition";
    case CL_INVALID_KERNEL:
        return "Invalid kernel";
    case CL_INVALID_ARG_INDEX:
        return "Invalid argument index";
    case CL_INVALID_ARG_VALUE:
        return "Invalid argument value";
    case CL_INVALID_ARG_SIZE:
        return "Invalid argument size";
    case CL_INVALID_KERNEL_ARGS:
        return "Invalid kernel arguments";
    case CL_INVALID_WORK_DIMENSION:
        return "Invalid work dimension";
    case CL_INVALID_WORK_GROUP_SIZE:
        return "Invalid work group size";
    case CL_INVALID_WORK_ITEM_SIZE:
        return "Invalid work item size";
    case CL_INVALID_GLOBAL_OFFSET:
        return "Invalid global offset";
    case CL_INVALID_EVENT_WAIT_LIST:
        return "Invalid event wait list";
    case CL_INVALID_EVENT:
        return "Invalid event";
    case CL_INVALID_OPERATION:
        return "Invalid operation";
    case CL_INVALID_GL_OBJECT:
        return "Invalid OpenGL object";
    case CL_INVALID_BUFFER_SIZE:
        return "Invalid buffer size";
    case CL_INVALID_MIP_LEVEL:
        return "Invalid mip-map level";
    default:
        return "Unknown";
    }
}


/*
 * checks if OpenCL error occured
 */

inline void checkClError(cl_int err, char* debug){
    if(err != CL_SUCCESS){
        cout << "ERROR: " << CLErrorString(err) << " " << debug << endl;
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    else {
        cout << "OK - " << debug << endl;
    }
#endif
}



void CL_CALLBACK contextCallback(const char *err_info,
                                 const void *private_intfo,
                                 size_t cb,
                                 void *user_data){
    cout << "ERROR during context use: " << err_info << endl;
    exit(EXIT_FAILURE);
}




/* 
 * Releases OpenCL resources (Context, Memory etc.) 
 */
int cleanup()
{

    //status = clReleaseKernel(dct_kernel);
    //checkClError(status, "clReleaseKernel dct_kernel.");

    error = clReleaseProgram(program);
    checkClError(error, "clReleaseProgram.");

    error = clReleaseCommandQueue(cl_queue);
    checkClError(error, "clReleaseCommandQueue.");

    error = clReleaseContext(context);
    checkClError(error, "clReleaseContext.");

    return 0;
}

















void GPU::test(){
    CBYTE src[] = {0x01, 0x02, 0x03, 0x04, 
                  0x05, 0x06, 0x07, 0x08,
                  0x09, 0x10, 0x11, 0x12,
                  0x13, 0x14, 0x15, 0x16};
    BYTE key[] = {0x05, 0x06, 0x07, 0x08,
                  0x01, 0x02, 0x03, 0x04,
                  0x09, 0x10, 0x11, 0x12,
                  0x13, 0x14, 0x15, 0x16};
    BYTE rk[11][16];
    AES::expand_key(key, rk);
    
    BYTE dst[16];

    /*sub_bytes(src, dst);
    print_state(dst);

    shift_rows(src, dst); 
    print_state(dst);

    mix_columns(src, dst);
    print_state(dst);
    
    xor_key(src, dst, key);
    print_state(dst);*/



    int length = 16;
    BYTE *iv;
    // Alloc buffers with specified size & copy data
    cl_int err;

    size_t packet_size = sizeof(cl_uchar)*length;

    cl_mem payload_src = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar)*length,
                NULL, &err);
    checkClError(err, "clCreateBuffer src");
    
    cl_mem payload_dst = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar)*length, 
                NULL, &err);
    checkClError(err, "clCreateBuffer dst");

    cl_mem key_gpu = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar)*16*11, NULL, &err);

    cl_mem temp = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar)*16, NULL, &err);
    checkClError(err, "clCreateBuffer key");
    
    
    // Copy data from memory to gpu
    err = clEnqueueWriteBuffer(cl_queue,
            payload_src, //memory on gpu
            CL_TRUE,   //blocking write
            0,         //offset
            sizeof(cl_uchar)*length, //size in bytes of copied data
            src,       //memory data
            0,         //wait list
            NULL,      //wait list
            NULL);     //wait list

    err = clEnqueueWriteBuffer(cl_queue,
            key_gpu, //memory on gpu
            CL_TRUE,   //blocking write
            0,         //offset
            sizeof(cl_uchar)*16*11, //size in bytes of copied data
            rk,       //memory data
            0,         //wait list
            NULL,      //wait list
            NULL);     //wait list

    clSetKernelArg(decode_kernel, 0, sizeof(cl_mem), (void *)&payload_src);
    clSetKernelArg(decode_kernel, 1, sizeof(cl_mem), (void *)&payload_dst);
    clSetKernelArg(decode_kernel, 2, sizeof(cl_mem), (void *)&key_gpu);
    clSetKernelArg(decode_kernel, 3, sizeof(cl_mem), (void *)&temp);

    clEnqueueNDRangeKernel(cl_queue, decode_kernel, 1, NULL, GWS, LWS, 0, NULL, NULL);
    
    // copy result back from gpu to host
    clEnqueueReadBuffer(cl_queue, payload_dst, CL_TRUE, 0, sizeof(cl_uchar)*16, 
                        dst, 0, NULL, NULL);

    AES::print_state(dst);


    clFinish(cl_queue);
}
