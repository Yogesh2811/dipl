/** @file */
#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <boost/thread.hpp>
#include <queue>
#include <map>

/** \brief Size of packet buffer*/
#define PACKET_SIZE 1024
/** \brief Size of buffer pool*/
#define POOL_SIZE 500

// forward declarations
class SRTP_stream;
class Parser_interface;
template<class t> class Buffer_pool;

typedef unsigned char BYTE;

/** \brief item for buffer pool with data buffers and socket structures*/
class RTP_item {
    public:
        BYTE src[PACKET_SIZE];  ///< incomming packet
	      BYTE *payload;          ///< payload pointer
        BYTE dst[PACKET_SIZE];  ///< outgoing packet
        BYTE temp[PACKET_SIZE]; ///< temporary buffer
    
        struct sockaddr_in6 src_addr;  ///< IPv6 struc
        struct iovec iov[1];           ///< I/O buffer
        struct msghdr msg;             ///< Message header

	      RTP_item();  ///< constructor
	      ~RTP_item(); ///< destructor
};

/** \brief Network interface for RTP packet transfers */
class RTP_interface {
    private:
        static int count; ///< number of RTP interfaces
        
        Parser_interface* p; ///< Pointer to parser for mutual interaction
        int id;              ///< ID of this interface
        int rtp_sock;        ///< RTP socket
        int rtcp_sock;       ///< RTCP socket
        struct sockaddr_in6 rtp_addr;      ///< IPv6 address of RTP socket
        struct sockaddr_in6 rtcp_addr;     ///< IPv6 address of RTCP socket
        bool exit;    ///< flag for exit
        
        std::map<unsigned int, SRTP_stream*> streams; ///< SRTP streams

	      Buffer_pool<RTP_item> *pool = NULL;     ///< buffer pool for packets
        /**
         * \brief parse incomming packet
         * 
         * @param id ID of the buffer in buffer pool
         * @param length length of the packet data                           
         */                 
        void parse_packet(int id, int length);  


    public:
        /**
         * \brief Constructor
         * 
         * @param p parser interface pointer
         * @param rtp_port port that socket will listen to RTP         
         * @param rtcp_port port that socket will listen to RTCP
         * @param err returned error if any                                    
         */ 
        RTP_interface(Parser_interface* p, int rtp_port, int rtcp_port, int* err);
        /** \brief destructor*/
        ~RTP_interface();
        /** \brief stop listening */
        void stop();
        /** \brief send data in buffer pool with specified ID
         *
         * @param id index to buffer pool
         * @param size length in bytes of outgoing data
         */                                   
        void send(int id, int size);
        /** \brief interface for upper layer - release SRTP stream*/ 
        void release_stream();
        /** \brief interface for upper layer - create SRTP stream*/
        SRTP_stream* create_stream();
        /** \brief interface for upper layer - find SRTP stream*/
        SRTP_stream* find_stream();
        /** \brief boost::thread interface method*/
        void operator()();
};

#endif
/** @} */