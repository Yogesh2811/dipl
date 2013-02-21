#include "srtp_stream.h"


SRTP_stream::SRTP_stream(Stream_type t){
    type = t; 
}

SRTP_stream::~SRTP_stream(){
}

SRTP_stream::Stream_type SRTP_stream::get_type(){
    return type;
}
