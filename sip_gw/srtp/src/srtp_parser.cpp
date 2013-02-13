#include "srtp_parser.h"
#include "aes.h"
#include "cl_util.h"
#include "rtp_interface.h"

#include <iostream>

using namespace std;

SRTP_parser::SRTP_parser(RTP_interface *iface, execution_type t){
    exit = false;

    i = iface;

    if(t == SERIAL_EXECUTION){
        encode = &srtp_encode;
        decode = &srtp_decode;
    }
    else{
        encode = &srtp_encode_gpu;
        decode = &srtp_decode_gpu;
    }
}


SRTP_parser::~SRTP_parser(){
}

void SRTP_parser::operator()(){
    while(!exit);
}

void SRTP_parser::quit(){
    exit = true;
}

void SRTP_parser::encode_msg(BYTE* in, BYTE* out, int id, int length){
    printf("SRTP_parser::encode_msg()\n");
}

void SRTP_parser::set_interface(RTP_interface *iface){
    i = iface;
}

void SRTP_parser::decode_msg(BYTE* in, BYTE* out, int id, int length){
    printf("SRTP_parser::decode_msg()\n");
    memcpy(out, in, length);
    out[length] = 'r';
    i->send(id,length+1);
}

