/** @file */
#ifndef SRTP_HEADER_H
#define SRTP_HEADER_H

#include <cstring>

typedef unsigned char BYTE;
typedef const BYTE CBYTE;

/**
 * /brief Header for SRTP packet 
 */ 
namespace SRTP{
    //little endian
    struct header {
        BYTE cc : 4;    ///< number of contributors
        BYTE x  : 1;    ///< extension
        BYTE p  : 1;    ///< padding
        BYTE v  : 2;    ///< version
     
        BYTE pt : 7;    ///< payload type
        BYTE m  : 1;    ///< marker
        
        unsigned short seq;      ///< sequential number
        unsigned int timestamp;  ///< timestamp
        unsigned int ssrc;       ///< synchronization source
        unsigned int csrc[15];   ///< contributing source
    };
    /**
     * \brief fixes header on different endian architectures, instead of using 
     * union which is tricky :)
     * 
     * @param h pointer to the header structure          
     */         
    void fix_header(header* h);
    /**
     * \brief finds payload in packet structure 
     * 
     * @param h pointer to the header structure
     * @param packet packet data
     * @param payload pointer to the start of payload in packet data
     * @returns length of payload                         
     */ 
    size_t get_payload(header* h, BYTE* packet, BYTE** payload);
    /**
     * \brief hepler function for endian swap
     * 
     * @param i swapped integer
     */                   
    unsigned int swap_int(unsigned int i);
    /**
     * \brief calculate initial vector for AES
     * 
     * @param k_s session salting key
     * @param ssrc synchronization source
     * @param packet_index index of the packet as defined by RFC3711
     * @param iv initial vector
     */                              
    void get_iv(CBYTE* k_s, unsigned int ssrc, CBYTE packet_index[6], BYTE iv[16]);
    /**
     * \brief calculate the packet index
     * 
     * @param seq sequential number
     * @param roc roll over counter
     * @param index index of the packet as defined by RFC3711
     */ 
    void get_packet_index(unsigned short seq, unsigned int roc, BYTE index[6]);
    /**
     * \brief hepler function for expanding integer to array of bytes
     * 
     * @param val integer value
     * @param b array of bytes     
     */ 
    void int_to_byte(unsigned int val, BYTE *b);
    /**
     * \brief hepler function for expanding short to array of bytes
     * 
     * @param val short value
     * @param b array of bytes     
     */ 
    void short_to_byte(unsigned short val, BYTE *b);
}

#endif
/** @} */