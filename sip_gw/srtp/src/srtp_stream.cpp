#include "srtp_stream.h"
#include "plugins.h"

SRTP_stream::SRTP_stream(Stream_type t){
    type = t; 
    state = INIT;
    roc = 0;
    alg = AES128_CTR;
    id = 0;

    memset(session_key, 0, 16);
}


SRTP_stream::~SRTP_stream(){
}

SRTP_stream::Stream_type SRTP_stream::get_type(){
    return type;
}

unsigned char* SRTP_stream::get_key(){
    return session_key;
}
