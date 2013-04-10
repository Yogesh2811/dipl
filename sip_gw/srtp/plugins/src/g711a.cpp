#include "transcode_plugin.h"

#include <cstdio>

//codec basic info
const char* encoding_name = "PCMA";
const int PT = 8;

//plugin interface functions
int transcode_function(const unsigned char* src, 
              unsigned char* dst, 
              int len_src, 
              int* len_dst, 
              int codec){
    printf("dynamic g711a\n");
    return 0;
}
int (*transcode)(const unsigned char* src, 
              unsigned char* dst, 
              int len_src, 
              int* len_dst, 
              int codec) = &transcode_function;

void to_raw(const unsigned char* src, unsigned char* raw, int len_src, int* len_dst){
}

void from_raw(const unsigned char* raw, unsigned char* dst, int len_src, int* len_dst){
}
