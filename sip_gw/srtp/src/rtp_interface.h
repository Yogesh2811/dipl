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
class SRTP_parser;

typedef unsigned char BYTE;


class RTP_interface {
    private:
        static int count;
        
        SRTP_parser* p;
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
        std::map<int, SRTP_stream*> streams;

        void init_msg_pool(int id);
        int get_buffer_id();
        void release_buffer(int id);
        void parse_packet(int id, int length);


    public:
        RTP_interface(SRTP_parser* p, int rtp_port, int rtcp_port, int* err);
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