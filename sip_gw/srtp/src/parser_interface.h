#ifndef PARSER_INTERFACE_H
#define PARSER_INTERFACE_H

#include "srtp_header.h"

typedef unsigned char BYTE;

class SRTP_stream;
class RTP_item;

class Parser_interface { 
    public:
        virtual void parse_msg(RTP_item* item, SRTP::header *h, SRTP_stream* s, int id, int length) = 0;
};


#endif
