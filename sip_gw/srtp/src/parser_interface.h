#ifndef PARSER_INTERFACE_H
#define PARSER_INTERFACE_H

#include "srtp_header.h"

typedef unsigned char BYTE;

class SRTP_stream;

class Parser_interface { 
    public:
        virtual void parse_msg(const BYTE* const in, SRTP::header* h, BYTE* out, SRTP_stream* s, int id, int length) = 0;
};


#endif
