#ifndef DAEMON_H
#define DAEMON_H

#include "parser_interface.h"

#define PARSER_COUNT 2

class SRTP_parser;
class SRTP_stream;
class RTP_interface;

typedef unsigned char BYTE;

class Daemon: public Parser_interface {
    public:
        Daemon(RTP_interface *r, SRTP_parser *parsers[PARSER_COUNT]);
        ~Daemon();
        void operator()();
        void stop();
        void parse_msg(const BYTE* in, BYTE* out, SRTP_stream* s, int id, int length);
        void set_interface(RTP_interface* i);

    private:
        bool quit;
        
        RTP_interface *r;
        SRTP_parser **parser;
};


#endif
