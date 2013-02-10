#ifndef SRTP_PARSER_H
#define SRTP_PARSER_H


typedef unsigned char BYTE;

class SRTP_stream;

class SRTP_parser { 
    public:
        enum execution_type{
            SERIAL_EXECUTION,
            PARALEL_EXECUTION
        };
        SRTP_parser(execution_type t);
        ~SRTP_parser();

        void quit();
        void encode_msg(BYTE* src, BYTE* dst, SRTP_stream* stream, int length);
        void decode_msg(BYTE* src, BYTE* dst, SRTP_stream* stream, int length);

        bool exit;

        void operator()();
    private:
        execution_type type;
        void (*encode)(BYTE* src, BYTE* dst, BYTE* key, BYTE* counter, int length);
        void (*decode)(BYTE* src, BYTE* dst, BYTE* key, BYTE* counter, int length);

};


#endif
