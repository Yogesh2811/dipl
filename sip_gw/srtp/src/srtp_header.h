#ifndef SRTP_HEADER_H
#define SRTP_HEADER_H

#include <cstring>

typedef unsigned char BYTE;
typedef const BYTE CBYTE;

namespace SRTP{
    //little endian
    struct header {
        BYTE cc : 4;
        BYTE x  : 1;
        BYTE p  : 1;
        BYTE v  : 2;
     
        BYTE pt : 7;
        BYTE m  : 1;
        
        unsigned short seq;
        unsigned int timestamp;
        unsigned int ssrc;
        unsigned int csrc[15];
    };
    void fix_header(header* h);
    size_t get_payload(header* h, BYTE* packet, BYTE** payload);
    unsigned int swap_int(unsigned int i);
    void get_iv(CBYTE* k_s, unsigned int ssrc, CBYTE packet_index[6], BYTE iv[16]);
    void get_packet_index(unsigned short seq, unsigned int roc, BYTE index[6]);
    void int_to_byte(unsigned int val, BYTE *b);
    void short_to_byte(unsigned short val, BYTE *b);
}

#endif
