/** @file */
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

#include <fstream>
#include <string>
#include <queue>

typedef unsigned char BYTE;
typedef const BYTE CBYTE;

cl_int loadKernelFromFile(const char* fileName, cl_kernel* kernel, char* kernel_name);
void checkClError(cl_int err, char* debug);
const char *CLErrorString(cl_int _err);


void CL_CALLBACK contextCallback(const char *err_info,
                                 const void *private_intfo,
                                 size_t cb,
                                 void *user_data);


/** \brief OpenCL implementation of AES */
namespace GPU {
    /** \brief initialization of OpenCL*/
    int initOpenCL();
    /** \brief Parallel encoding of entire packet payload using 128-bit AES in 
    *  CTR mode
    *    
    * @param src source data buffer with incomming packet payload
    * @param dst destination data buffer with outgoing packet payload 
    * @param key encryption master key defined by the session
    * @param iv initial vector for CTR mode
    * @param length number of bytes of incomming packet payload                   
    */
    void srtp_decode_gpu(BYTE* src, BYTE* dst, BYTE* key, BYTE* iv, int length);
    /** \brief Parallel decoding of entire packet payload using 128-bit AES in 
    *  CTR mode
    *    
    * @param src source data buffer with incomming packet payload
    * @param dst destination data buffer with outgoing packet payload 
    * @param key encryption master key defined by the session
    * @param iv initial vector for CTR mode
    * @param length number of bytes of incomming packet payload                   
    */
    void srtp_encode_gpu(BYTE* src, BYTE* dst, BYTE* key, BYTE* iv, int length);
    /** \brief dealocation of OpenCL*/
    int cleanup();
    void test();
}

#endif
/** @} */