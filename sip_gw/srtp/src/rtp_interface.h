#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <boost/thread.hpp>
#include <queue>
#include <map>

#define PACKET_SIZE 1024
#define POOL_SIZE 10

class SRTP_stream;
class Parser_interface;

typedef unsigned char BYTE;

namespace RTP{
    //little endian
    struct header {
        BYTE cc : 4;
        BYTE x  : 1;
        BYTE p  : 1;
        BYTE v  : 2;
     
        BYTE pt : 7;
        BYTE m  : 1;
        
        unsigned short seq;
        unsigned int timestamp;
        unsigned int ssrc;
        unsigned int csrc[15];
    };
    void fix_header(header* h);
    size_t get_payload(header* h, BYTE* packet, BYTE* payload);
    unsigned int swap_int(unsigned int i);
}

class RTP_interface {
    private:
        static int count;
        
        Parser_interface* p;
        int id;
        int rtp_sock;
        int rtcp_sock;
        struct sockaddr_in6 rtp_addr;
        struct sockaddr_in6 rtcp_addr;
        bool exit;
        
        unsigned char buffer_pool[POOL_SIZE][PACKET_SIZE];
        unsigned char out_buffer_pool[POOL_SIZE][PACKET_SIZE];
        struct sockaddr_in6 src_addr_pool[POOL_SIZE];
        struct iovec iov_pool[POOL_SIZE][1];
        struct msghdr msg_pool[POOL_SIZE];
        std::queue<int> free_buffer_index;
        std::map<unsigned int, SRTP_stream*> streams;

        void init_msg_pool(int id);
        int get_buffer_id();
        void release_buffer(int id);
        void parse_packet(int id, int length);


    public:
        RTP_interface(Parser_interface* p, int rtp_port, int rtcp_port, int* err);

        ~RTP_interface();

        void stop();
        void send(int id, int size);
        void release_stream();
        SRTP_stream* create_stream();
        SRTP_stream* find_stream();
        //void set_parser(SRTP_parser* p);
        void operator()();
};




#endif
