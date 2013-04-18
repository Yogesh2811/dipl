#ifndef SRTP_STREAM_H
#define SRTP_STREAM_H

#define LIST_SIZE 10

#include "rtp_interface.h"

class SRTP_stream {
    public:
        enum Stream_type {
            FORWARD,
            ENCODE,
            DECODE,
            ENCODE_DECODE
        };

        enum Status {
            INIT,
            SESSION,
            END
        };

        enum Encryption {
            AES128_CTR
        };

        enum Transcoding {
            NO_TRANSCODE,
            TRANSCODE_NAIVE,
            TRANSCODE_FUNCTION
        };

        SRTP_stream(Stream_type type);
        ~SRTP_stream();
        Stream_type get_type();
        const unsigned char* get_key();
    
        unsigned int roc;
    
    private:
        Status state;
        Stream_type type;
        unsigned int highest_seq;
        Encryption alg;
        BYTE master_key[16];
        unsigned int master_key_counter;
        BYTE session_key[16];
        
        BYTE replay_list[LIST_SIZE][PACKET_SIZE];
        BYTE master_salt[16];
        unsigned int MKI;

        //transcoding
        Transcoding tr_state;
        int src_pt;
        int dst_pt;
        void* transcode_fun;
};


#endif
