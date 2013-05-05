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

void SRTP_parser::set_interface(RTP_interface *iface){
    i = iface;
}

void SRTP_parser::parse_msg(RTP_item* item, SRTP::header* h, SRTP_stream* s, int id, int len){
    LOG_MSG("SRTP_parser::parse_msg()");

    //1) find counter and key for encryption
    //2) get counter and expand to be the same length as input aligned to 128bits
    //3) send for processing
    //4) send output
    
    //int size = (ceil(len/16.0))*16;
    BYTE pi[6];
    BYTE iv[16] = {0};
    SRTP::get_packet_index(s->roc, h->seq, pi);
    SRTP::get_iv(nullptr, h->ssrc, pi, iv);
    
    BYTE *key =  s->get_key();

    //transcoding temp vars
    int l_dst;

    switch(s->get_type()){
        case SRTP_stream::ENCODE :
            encode(item->src, item->dst, key, iv, len);
            break;
        case SRTP_stream::DECODE :
            decode(item->src, item->dst, key, iv, len);
            break;
        case SRTP_stream::TRANSCODE_ENCODE:
            Plugins::transcode(item->src, item->temp, len, &l_dst, s->src_pt, s->dst_pt, s->id);
            encode(item->temp, item->dst, key, iv, l_dst);
            break;
        case SRTP_stream::DECODE_TRANSCODE:
            decode(item->src, item->temp, key, iv, len);
            Plugins::transcode(item->temp, item->dst, len, &l_dst, s->src_pt, s->dst_pt, s->id);
            break;
        case SRTP_stream::DECODE_TRANSCODE_ENCODE:
            decode(item->src, item->dst, key, iv, len);
            Plugins::transcode(item->dst, item->temp, len, &l_dst, s->src_pt, s->dst_pt, s->id);
            encode(item->temp, item->dst, key, iv, l_dst);
            break;
        default: //forward
            memcpy(item->src, item->dst, len);
            break;
    };
    
    /*char sseq[10];
    sprintf(sseq, "%d", h->seq);
    memcpy(out,sseq,10);*/
    //printf("[%s] %d [%d]\n", out, len, h->seq);
    i->send(id,len);
}

