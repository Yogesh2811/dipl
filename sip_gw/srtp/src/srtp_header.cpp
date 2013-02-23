#include "srtp_header.h"

#include <netinet/in.h>

using namespace std;

//ntohl()
unsigned int SRTP::swap_int(unsigned int i){
    return (i >> 24) | ((i<<8) & 0x00FF0000) | ((i>>8) & 0x0000FF00) | (i << 24);
}

void SRTP::fix_header(SRTP::header* h){
    h->seq = ntohs(h->seq);//h->seq >> 8 | h->seq << 8;
    h->timestamp = SRTP::swap_int(h->timestamp);
    h->ssrc = ntohl(h->ssrc);//SRTP::swap_int(h->ssrc);
    if(h->cc > 0){
        for(int i=0; i<h->cc; i++){
            h->csrc[i] = ntohl(h->csrc[i]);//SRTP::swap_int(h->csrc[i]);
        }
    }
}

size_t SRTP::get_payload(SRTP::header* h, BYTE* packet, BYTE** payload){
    size_t header_size = sizeof(SRTP::header) - (15-h->cc)*sizeof(unsigned int);
    (*payload) = packet + header_size;
    return header_size;
}


void SRTP::get_packet_index(unsigned short seq, unsigned int roc, BYTE index[6]){
    int_to_byte(roc, index);
    short_to_byte(seq, index+4);
}

/*
 * k_s -- salt key
 * SSRC
 * packet_index
 * output -- initial value
 */
void SRTP::get_iv(CBYTE *k_s, unsigned int ssrc, CBYTE packet_index[6], BYTE iv[16]){
    //TODO salt
    int_to_byte(ssrc, iv+4);
    memcpy(iv+8,packet_index,6);
}

void SRTP::int_to_byte(unsigned int num, BYTE *b){
    num = htonl(num);
    b[0] = num >> 24;
    b[1] = (num & 0x00FF0000) >> 16;
    b[2] = (num & 0x0000FF00) >> 8;
    b[3] = num;
}

void SRTP::short_to_byte(unsigned short num, BYTE *b){
    num = htons(num);
    b[0] = num >> 8;
    b[1] = num;
}

