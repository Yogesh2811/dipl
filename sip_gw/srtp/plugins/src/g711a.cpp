#include "transcode_plugin.h"
#include <cstdio>
#include <cstring>
#include <netinet/in.h>

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
#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	NSEGS		(8)		/* Number of A-law segments. */
#define	SEG_SHIFT	(4)		/* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */

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

static short seg_aend[8] = {0x1F, 0x3F, 0x7F, 0xFF,
			    0x1FF, 0x3FF, 0x7FF, 0xFFF};

#define ALAW_MAX 0xFFF


void transcode_g711u(CBYTE* src, BYTE* dst, int length){
    for(int i = 0; i<length; i++){
        dst[i]=((src[i]&0x80)?(0xFF^a2u[src[i]^0xD5]):(0x7F^a2u[src[i]^0x55]));
    }
}

short search(short val, short *table, short size){
    for(short i = 0; i<size; i++) {
        if(val <= *table++)
	    return(i);
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

short alaw_to_pcm(BYTE number){
    BYTE sign = 0x00,  position = 0;
    short decoded = 0;
    number ^= 0x55;

    if(number&0x80){
	number &= ~(1<<7);
	sign = -1;
    }
    position = ((number & 0xF0) >> 4) + 4;
    if(position != 4){
	decoded = ((1<<position)|((number&0x0F)<<(position-4))
	          |(1<<(position-5)));
    }
    else{
	decoded = (number<<1)|1;
    }
    return (sign==0)?(decoded):(-decoded);
}

void to_raw_f(CBYTE* src, BYTE* raw, int len_src, int* len_dst){
    (*len_dst) = len_src*2;
    BYTE aval;
    short t, seg, pcm;

    for(int i = 0, ii = 0; i<len_src; i++, ii+=2){
	pcm = alaw_to_pcm(src[i]);
	raw[ii]   = (pcm >> 8); ;
	raw[ii+1] = pcm;
    }
}

BYTE pcm_to_alaw(short pcm){
    short mask = 0x800;
    BYTE sign = 0, position = 11, lsb = 0;
    
    if(pcm < 0){
	pcm = -pcm;    
	sign = 0x80;
    }
    if(pcm > ALAW_MAX){
	pcm = ALAW_MAX;
    }

    for(; ((pcm & mask)!=mask && position >= 5); mask >>=1, position--);
    lsb = (pcm >> ((position==4)?(1):(position-4))) & 0x0F;
    return (sign | ((position-4)<<4)|lsb)^0x55;
}

void from_raw_f(CBYTE* raw, BYTE* dst, int len_src, int* len_dst){
    (*len_dst) = len_src/2;
    short pcm, seg, mask;
    BYTE aval;

    for(int i = 0, ii = 0; i<len_src; i+=2, ii++){
	pcm = ((short)raw[i])<<8 | (short)raw[i+1];
	dst[ii] = pcm_to_alaw(pcm);
    }
}




