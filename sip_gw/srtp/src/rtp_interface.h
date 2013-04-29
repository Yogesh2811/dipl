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
//template<class t> class Buffer_pool;

typedef unsigned char BYTE;

class RTP_item {
    public:
        BYTE src[PACKET_SIZE];
	BYTE *payload;
        BYTE dst[PACKET_SIZE];
        BYTE temp[PACKET_SIZE];
    
        struct sockaddr_in6 src_addr;
        struct iovec iov[1];
        struct msghdr msg;

	void init();
};

template <class buffer_item> class Buffer_pool {
    private:
        buffer_item **pool = NULL;
        std::queue<int> free_buffer_index;
	int pool_size;

    public:
        Buffer_pool(int pool_size) {
	    pool = (buffer_item**)malloc(sizeof(buffer_item)*pool_size);
            for(int i=0; i<pool_size; i++){
		pool[i] = new buffer_item();
		free_buffer_index.push(i);
	    }
	}
        ~Buffer_pool() {
	    if(pool != NULL){ 
		free(pool);
		for(int i = 0; i<pool_size; i++){
		    if(pool[i] != NULL) delete pool[i];
		}
	    }
	}
        int get_buffer_id() {
	    while(free_buffer_index.empty());

	    int id = free_buffer_index.front(); 
	    free_buffer_index.pop();
	    return id; 
	}
	int get_pool_size(){
            return pool_size;
	}
        
	buffer_item* get_item(int id) {
	    return pool[id];
	}
        void release_buffer(int id) {
	    free_buffer_index.push(id);
	}
};

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
        
        //unsigned char buffer_pool[POOL_SIZE][PACKET_SIZE];
        //unsigned char out_buffer_pool[POOL_SIZE][PACKET_SIZE];
        //struct sockaddr_in6 src_addr_pool[POOL_SIZE];
        //struct iovec iov_pool[POOL_SIZE][1];
        //struct msghdr msg_pool[POOL_SIZE];
        //std::queue<int> free_buffer_index;
        std::map<unsigned int, SRTP_stream*> streams;

	Buffer_pool<RTP_item>* pool = NULL;
	
        //void init_msg_pool(int id);
        //int get_buffer_id();
        //void release_buffer(int id);
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
