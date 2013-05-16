/** @file */
#ifndef SRTP_STREAM_H
#define SRTP_STREAM_H

#define LIST_SIZE 10

#include "rtp_interface.h"

/** \brief SRTP stream class for interface with upper layer*/
class SRTP_stream {
    public:
        /** \brief processing of the stream definition*/
        enum Stream_type {
            FORWARD,
            ENCODE,
            DECODE,
            TRANSCODE_ENCODE,
            DECODE_TRANSCODE,
            DECODE_TRANSCODE_ENCODE,
        };
        /** \brief status of the stream */
        enum Status {
            INIT,
            SESSION,
            END
        };
        /** \brief encryption type*/
        enum Encryption {
            AES128_CTR
        };
        /** 
         * \brief constructor 
         * 
         * @param type processing of the stream definition
         */
        SRTP_stream(Stream_type type);
        /** \brief destructor*/
        ~SRTP_stream();
        /** \brief returns type of the stream*/
        Stream_type get_type();
        /** \brief set transcoding type*/
        void set_transcoding();
        /** \brief returns master key*/
        unsigned char* get_key();

        unsigned int roc; ///< rollover counter
    
        //transcoding
        int src_pt;       ///< payload type of incomming packets
        int dst_pt;       ///< payload type of outgoing packets
        int id;           ///< stream id
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
};

#endif
/** @} */