/** @file */
#ifndef SRTP_PARSER_H
#define SRTP_PARSER_H

#include <map>

#include "parser_interface.h"
#include "plugins.h"

//forward declarations
class RTP_interface;
class SRTP_stream;
class RTP_item; 

/** \brief SRTP parser for parsing the network packet data*/
class SRTP_parser: public Parser_interface { 
    public:
        /** \brief type of implementation of packet processing*/
        enum execution_type{
            SERIAL_EXECUTION,             ///< uses serial implementation of AES
            PARALEL_EXECUTION,            ///< uses paral. implementation of AES
            PERSISTENT_THREAD_EXECUTION,  ///< uses PT implementation of AES
        };
        /** \brief Constructor
         * 
         * @param i network interface
         * @param t execution type         
         */
        SRTP_parser(RTP_interface *i, execution_type t);
        /** \brief Destructor*/
        ~SRTP_parser();

        /** \brief stopping the parser*/
        void quit();
        /** \brief sets the network interface*/
        void set_interface(RTP_interface *iface);
        /** \brief sends data from network interface for parsing
         *
         * @param item object with data buffers
         * @param h SRTP header
         * @param s SRTP stream
         * @param id buffer pool id
         * @param length of source payload
         */   
        void parse_msg(RTP_item* item, SRTP::header *h, SRTP_stream* s, int id, int length);

        bool exit;  ///< flag of exit
        /** \brief boost::thread interface method*/
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
/** @} */