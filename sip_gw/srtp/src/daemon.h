/** @file */
#ifndef DAEMON_H
#define DAEMON_H

//interface for SRTP parsing
#include "parser_interface.h"

/** \brief Number of SRTP parsers*/
#define PARSER_COUNT 2

//forward declarations
class SRTP_parser;
class SRTP_stream;
class RTP_interface;
class RTP_item;

typedef unsigned char BYTE;

/** \brief Class for dynamic SRTP parses allocation*/
class Daemon: public Parser_interface {
    public:
        /** \brief Constructor
         * @param r network interface
         * @param parsers array of SRTP parsers
         */
        Daemon(RTP_interface *r, SRTP_parser *parsers[PARSER_COUNT]);
        /** \brief Destructor*/
        ~Daemon();
        /** \brief boost::thread interface method*/
        void operator()();
        /** \brief stopping the thread*/
        void stop();
        /** \brief sends data from network interface to the SRTP parser
         *
         * @param item object with data buffers
         * @param h SRTP header
         * @param s SRTP stream
         * @param id buffer pool id
         * @param length of source payload
         */                                                              
        void parse_msg(RTP_item *item, SRTP::header *h, SRTP_stream* s, 
                       int id, int length);
        /** 
         * \brief sets network interface
         *
         * @param i network interface
         */
        void set_interface(RTP_interface* i);

    private:
        bool quit;
        
        RTP_interface *r;
        SRTP_parser **parser;
};

#endif
/** @} */