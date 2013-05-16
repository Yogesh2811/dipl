/** @file */
#ifndef PARSER_INTERFACE_H
#define PARSER_INTERFACE_H

#include "srtp_header.h"

typedef unsigned char BYTE;

//forward declarations
class SRTP_stream;
class RTP_item;

/** \brief virtual class for SRTP parser interface*/
class Parser_interface { 
    public:
        /** \brief sends data from network interface to the SRTP parser
         *
         * @param item object with data buffers
         * @param h SRTP header
         * @param s SRTP stream
         * @param id buffer pool id
         * @param length of source payload
         */  
        virtual void parse_msg(RTP_item* item, SRTP::header *h, SRTP_stream* s, int id, int length) = 0;
};

#endif
/** @} */