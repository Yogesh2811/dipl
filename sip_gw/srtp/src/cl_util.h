#ifndef CL_UTIL_H
#define CL_UTIL_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

#include <iostream>
#include <fstream>
#include <string>
#include <queue>

typedef unsigned char BYTE;
typedef const BYTE CBYTE;

int initOpenCL();
cl_int loadKernelFromFile(const char* fileName, cl_kernel* kernel, char* kernel_name);
void checkClError(cl_int err, char* debug);
const char *CLErrorString(cl_int _err);


void CL_CALLBACK contextCallback(const char *err_info,
                                 const void *private_intfo,
                                 size_t cb,
                                 void *user_data);


void srtp_decode_gpu(CBYTE* src, BYTE* dst, CBYTE* key, CBYTE* iv, int length);
void srtp_encode_gpu(CBYTE* src, BYTE* dst, CBYTE* key, CBYTE* iv, int length);


int cleanup();

namespace GPU {
    int alloc_buffer();
    void release_buffer(int id); 
    void test();
}

#endif
