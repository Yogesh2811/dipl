#include "rtp_interface.h"
#include "srtp_stream.h"
#include "parser_interface.h"
#include "utils.h"
#include "srtp_header.h"
#include "buffer_pool.h"

#include <iostream>

using namespace std;

int RTP_interface::count = 0;


RTP_interface::RTP_interface(Parser_interface* parser, int rtp, int rtcp, int* err){
    LOG_MSG("RTP_interface()")
    *err = 0;
    p = parser;
    id = count++;
    exit = false;
    
    rtp_addr.sin6_port    = htons(rtp);
    rtp_addr.sin6_family  = AF_INET6;
    rtp_addr.sin6_addr    = IN6ADDR_ANY_INIT;

    rtcp_addr.sin6_port   = htons(rtcp);
    rtcp_addr.sin6_family = AF_INET6;
    rtcp_addr.sin6_addr   = IN6ADDR_ANY_INIT; 
    
    rtp_sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if(rtp_sock == -1)
        *err = 1;
    if (bind(rtp_sock, (sockaddr*)&rtp_addr, sizeof(rtp_addr))==-1)
        *err = 1;

    rtcp_sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if(rtcp_sock == -1)
        *err = 2;
    if (bind(rtcp_sock, (sockaddr*)&rtcp_addr, sizeof(rtcp_addr))==-1)
        *err = 2;

    
    pool = new Buffer_pool<RTP_item>(POOL_SIZE);
    //item = new RTP_item();
    /*for(int i = 0; i<POOL_SIZE; i++){
	pool->get_item(i)->init();
    }*/
}

RTP_interface::~RTP_interface(){
    close(rtp_sock);
    close(rtcp_sock);

    for(auto stream : streams){
        delete stream.second;
    }

    //if(pool != NULL) delete pool;
}

RTP_item::RTP_item(){
    iov[0].iov_base=src;
    iov[0].iov_len=PACKET_SIZE;

    msg.msg_name=&(src_addr);
    msg.msg_namelen=sizeof(src_addr);
    msg.msg_iov=iov;
    msg.msg_iovlen=1;
    msg.msg_control=0;
    msg.msg_controllen=0;
}

RTP_item::~RTP_item() {
}

void RTP_interface::stop() {
    LOG_MSG("RTP_interface::stop()")
    exit = true;
}

void RTP_interface::parse_packet(int id, int length){
    //1) get SSRC and CSRC from packet
    //2) find stream or create new stream
    //3) get payload
    //4) send signal to the stream for parsing

    LOG_MSG("RTP_interface::parse_packet()")

    RTP_item* i = pool->get_item(id);
    SRTP::header* rtp_h = (SRTP::header*)i->src;
    SRTP::fix_header(rtp_h);

    size_t header_size = SRTP::get_payload(rtp_h, i->src, &(i->payload));


    if(streams[rtp_h->ssrc] == NULL){
        streams[rtp_h->ssrc] = new SRTP_stream(SRTP_stream::DECODE);
    }
    
    p->parse_msg(i, rtp_h, streams[rtp_h->ssrc], id, length-header_size);
}

/**
 * Send method is called by SRTP parsing module after the out_buffer_pool[id] is filled 
 * with data to send back to user
 *
 * @param id -- id of buffer from pool
 * @param size -- size of outgoing data in bytes data
 */
void RTP_interface::send(int id, int size){
    // 1) send msg saved in out_buffer_pool[id]
    // 2) release buffer id used for this packet
    // release_buffer(id);
    LOG_MSG("RTP_interface::send(id=%d, size=%d)", id, size);
	 
    RTP_item* i = pool->get_item(id);

    int err = sendto(rtp_sock, i->dst, size, 0, 
    (sockaddr*)&(i->src_addr), sizeof(i->src_addr));
    //pool->release_buffer(id);
}

void RTP_interface::operator()(){
    LOG_MSG("RTP_interface::capturing");
    int id, bytes;
    RTP_item* item;
    while(!exit){
        id = pool->get_buffer_id();
        item = pool->get_item(id);
        bytes = recvmsg(rtp_sock, &(item->msg), 0);
        if(bytes > 0){
            parse_packet(id, bytes);
        }
    }
}
