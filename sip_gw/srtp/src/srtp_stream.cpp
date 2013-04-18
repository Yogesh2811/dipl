#include "srtp_stream.h"
#include "plugins.h"

SRTP_stream::SRTP_stream(Stream_type t){
    type = t; 
    state = INIT;
    roc = 0;
    alg = AES128_CTR;
    
    memset(session_key, 0, 16);

    transcode_function = NULL;
}


SRTP_stream::~SRTP_stream(){
}

/*void SRTP_stream::set_transcoding(int src_PT, int dst_PT){

}*/

SRTP_stream::Stream_type SRTP_stream::get_type(){
    return type;
}

const unsigned char* SRTP_stream::get_key(){
    return session_key;
}
