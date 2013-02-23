#include "srtp_parser.h"
#include "aes.h"
#include "cl_util.h"
#include "rtp_interface.h"
#include "parser_interface.h"
#include "srtp_stream.h"
#include "utils.h"
#include <math.h>


SRTP_parser::SRTP_parser(RTP_interface *iface, execution_type t){
    exit = false;

    i = iface;

    if(t == SERIAL_EXECUTION){
        encode = &AES::srtp_encode;
        decode = &AES::srtp_decode;
        //encode_decode = &srtp_encode_decode;
    }
    else{
        encode = &srtp_encode_gpu;
        decode = &srtp_decode_gpu;
        //encode_decode = &srtp_encode_decode_gpu;
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

void SRTP_parser::set_interface(RTP_interface *iface){
    i = iface;
}

void SRTP_parser::parse_msg(const BYTE* in, SRTP::header *h, BYTE* out, SRTP_stream* s, 
                            int id, int len){
    LOG_MSG("SRTP_parser::parse_msg()");
    
    //1) find counter and key for encryption
    //2) get counter and expand to be the same length as input aligned to 128bits
    //3) send for processing
    //4) send output
    
    //int size = (ceil(length/16.0))*16;
    BYTE pi[6];
    BYTE iv[16];
    SRTP::get_packet_index(s->roc, h->seq, pi);
    SRTP::get_iv(nullptr, h->ssrc, pi, iv);
    
    BYTE key[16];


    switch(s->get_type()){
        case SRTP_stream::ENCODE :
            encode(in, out, key, iv, len);
            break;
        case SRTP_stream::DECODE :
            decode(in, out, key, iv, len);
            break;
        case SRTP_stream::ENCODE_DECODE : 
            encode_decode(in, out, key, iv, len);
            break;
        default: //forward
            memcpy(out, in, len);
            break;
    };
    i->send(id,len);
}

