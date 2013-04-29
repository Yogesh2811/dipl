#ifndef SRTP_PARSER_H
#define SRTP_PARSER_H

#include <map>

#include "parser_interface.h"
#include "plugins.h"

class RTP_interface;
class SRTP_stream;
class RTP_item; 

class SRTP_parser: public Parser_interface { 
    public:
        enum execution_type{
            SERIAL_EXECUTION,
            PARALEL_EXECUTION,
            PERSISTENT_THREAD_EXECUTION,
        };
        SRTP_parser(RTP_interface *i, execution_type t);
        ~SRTP_parser();

        void quit();
        void set_interface(RTP_interface *iface);
        void parse_msg(RTP_item* item, SRTP::header *h, SRTP_stream* s, int id, int length);

        bool exit;

        void operator()();


    private:
        execution_type type;
        RTP_interface *i;
        void (*encode)(BYTE* in, BYTE* out, BYTE* key, 
                       BYTE* counter, int length);
        void (*decode)(BYTE* in, BYTE* out, BYTE* key,
                       BYTE* counter, int length);
};


#endif
