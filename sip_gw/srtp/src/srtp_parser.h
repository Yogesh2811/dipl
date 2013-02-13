#ifndef SRTP_PARSER_H
#define SRTP_PARSER_H

#include <map>

typedef unsigned char BYTE;

class RTP_interface;
class SRTP_stream;

class SRTP_parser { 
    public:
        enum execution_type{
            SERIAL_EXECUTION,
            PARALEL_EXECUTION
        };
        SRTP_parser(RTP_interface *i, execution_type t);
        ~SRTP_parser();

        void quit();
        void set_interface(RTP_interface *iface);
        void encode_msg(BYTE* in, BYTE* out, int id, int length);
        void decode_msg(BYTE* in, BYTE* out, int id, int length);

        bool exit;

        void operator()();


    private:
        execution_type type;
        RTP_interface *i;
        void (*encode)(BYTE* in, BYTE* out, BYTE* key, BYTE* counter, int length);
        void (*decode)(BYTE* in, BYTE* out, BYTE* key, BYTE* counter, int length);

};


#endif
