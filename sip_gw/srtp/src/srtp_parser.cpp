#include "srtp_parser.h"
#include "aes.h"
#include "cl_util.h"


SRTP_parser::SRTP_parser(execution_type t){
    exit = false;
    
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

void encode_msg(BYTE* src, BYTE* dst, SRTP_stream* stream, int length){
    printf("SRTP_parser::encode_msg()");
}

void decode_msg(BYTE* src, BYTE* dst, SRTP_stream* stream, int length){
    printf("SRTP_parser::decode_msg()");
}

