#include "srtp_stream.h"


SRTP_stream::SRTP_stream(Stream_type t){
    type = t; 
    state = INIT;
    roc = 0;
    alg = AES128_CTR;
}


SRTP_stream::~SRTP_stream(){
}

SRTP_stream::Stream_type SRTP_stream::get_type(){
    return type;
}
