#include "transcode_plugin.h"
#include <cstdio>

//codec basic info
const char* encoding_name = "PCMU";
const int PT = 0;

//functions for plugin interface
int transcode_f(CBYTE* src, BYTE* dst, int l_src, int* l_dst, int pt){
    printf("dynamic %s %d\n", encoding_name, PT);
    return 0;
}

void to_raw_f(CBYTE* src, BYTE* raw, int len_src, int* len_dst){
}

void from_raw_f(CBYTE* raw, BYTE* dst, int len_src, int* len_dst){
}




//public interface of the plugin
int (*transcode)(CBYTE*, BYTE*, int, int*, int) = &transcode_f;
void (*to_raw)(CBYTE*, BYTE*, int, int*) = &to_raw_f;
void (*from_raw)(CBYTE*, BYTE*, int, int*) = &from_raw_f;
