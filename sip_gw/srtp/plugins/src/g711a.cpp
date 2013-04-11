#include "transcode_plugin.h"
#include <cstdio>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////
//                                    INTERFACE                                           //
////////////////////////////////////////////////////////////////////////////////////////////
//codec basic plugin info
const char* encoding_name = "PCMA";
const int PT = 8;

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

/* A- to u-law conversions */
static CBYTE a2u[128] = {         
1,   3,   5,   7,   9,   11,  13,  15,
16,  17,  18,  19,  20,  21,  22,  23,
24,  25,  26,  27,  28,  29,  30,  31,
32,  32,  33,  33,  34,  34,  35,  35,
36,  37,  38,  39,  40,  41,  42,  43,
44,  45,  46,  47,  48,  48,  49,  49,
50,  51,  52,  53,  54,  55,  56,  57,
58,  59,  60,  61,  62,  63,  64,  64,
65,  66,  67,  68,  69,  70,  71,  72,
73,  74,  75,  76,  77,  78,  79,  80,
80,  81,  82,  83,  84,  85,  86,  87,
88,  89,  90,  91,  92,  93,  94,  95,
96,  97,  98,  99,  100, 101, 102, 103,
104, 105, 106, 107, 108, 109, 110, 111,
112, 113, 114, 115, 116, 117, 118, 119,
120, 121, 122, 123, 124, 125, 126, 127};


void transcode_g711u(CBYTE* src, BYTE* dst, int length){
    for(int i = 0; i<length; i++){
        dst[i] = ((src[i] & 0x80) ? (0xFF ^ a2u[src[i] ^ 0xD5]) : (0x7F ^ a2u[src[i] ^ 0x55]));
    }
}

int transcode_f(CBYTE* src, BYTE* dst, int l_src, int* l_dst, int pt){
    if(pt == 8){
        *l_dst = l_src;
        memcpy(dst, src, sizeof(BYTE)*l_src);
        return 1;
    }
    else if(pt == 0){
        *l_dst = l_src;
        transcode_g711u(src, dst, l_src);
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




