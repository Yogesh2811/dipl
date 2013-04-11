#include "transcode_plugin.h"
#include <cstdio>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////
//                                    INTERFACE                                           //
////////////////////////////////////////////////////////////////////////////////////////////
//codec basic info
const char* encoding_name = "PCMU";
const int PT = 0;

//functions for plugin interface
int transcode_f(CBYTE* src, BYTE* dst, int l_src, int* l_dst, int pt);
void to_raw_f(CBYTE* src, BYTE* raw, int len_src, int* len_dst);
void from_raw_f(CBYTE* raw, BYTE* dst, int len_src, int* len_dst);

//public interface of the plugin
int (*transcode)(CBYTE*, BYTE*, int, int*, int) = &transcode_f;
void (*to_raw)(CBYTE*, BYTE*, int, int*) = &to_raw_f;
void (*from_raw)(CBYTE*, BYTE*, int, int*) = &from_raw_f;




////////////////////////////////////////////////////////////////////////////////////////////
//                                   IMPLEMENTATION                                       //
////////////////////////////////////////////////////////////////////////////////////////////
/* u- to A-law conversions */
BYTE u2a[128] = {         
1,   1,   2,   2,   3,   3,   4,   4,
5,   5,   6,   6,   7,   7,   8,   8,
9,   10,  11,  12,  13,  14,  15,  16,
17,  18,  19,  20,  21,  22,  23,  24,
25,  27,  29,  31,  33,  34,  35,  36,
37,  38,  39,  40,  41,  42,  43,  44,
46,  48,  49,  50,  51,  52,  53,  54,
55,  56,  57,  58,  59,  60,  61,  62,
64,  65,  66,  67,  68,  69,  70,  71,
72,  73,  74,  75,  76,  77,  78,  79,
80,  82,  83,  84,  85,  86,  87,  88,
89,  90,  91,  92,  93,  94,  95,  96,
97,  98,  99,  100, 101, 102, 103, 104,
105, 106, 107, 108, 109, 110, 111, 112,
113, 114, 115, 116, 117, 118, 119, 120,
121, 122, 123, 124, 125, 126, 127, 128};


void transcode_g711a(CBYTE* src, BYTE* dst, int length){
    for(int i = 0; i<length; i++){
        dst[i] = ((src[i] & 0x80) ? (0xD5 ^ u2a[src[i] ^ 0xFF] - 1) : (0x55 ^ u2a[src[i] ^ 0x7F] - 1));
    }
}

int transcode_f(CBYTE* src, BYTE* dst, int l_src, int* l_dst, int pt){
    if(pt == 0){
        *l_dst = l_src;
        memcpy(dst, src, sizeof(BYTE)*l_src);
        return 1;
    }
    else if(pt == 8){
        *l_dst = l_src;
        transcode_g711a(src, dst, l_src);
        return 1;
    }
    return 0;
}

void to_raw_f(CBYTE* src, BYTE* raw, int len_src, int* len_dst){
    printf("dynamic to_raw %s %d\n", encoding_name, PT);
}

void from_raw_f(CBYTE* raw, BYTE* dst, int len_src, int* len_dst){
    printf("dynamic from_raw %s %d\n", encoding_name, PT);
}





