#include "rtp_interface.h"
#include "srtp_stream.h"
#include "parser_interface.h"
#include "utils.h"
#include "srtp_header.h"

#include <iostream>

using namespace std;

int RTP_interface::count = 0;


RTP_interface::RTP_interface(Parser_interface* parser, int rtp, int rtcp, int* err){
    err = 0;
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

    for(int i = 0; i<POOL_SIZE; i++){
        free_buffer_index.push(i);
        init_msg_pool(i);
    }
}

RTP_interface::~RTP_interface(){
    close(rtp_sock);
    close(rtcp_sock);

    for(auto stream : streams){
        delete stream.second;
    }
}

void RTP_interface::init_msg_pool(int id){
    iov_pool[id][0].iov_base=buffer_pool[id];
    iov_pool[id][0].iov_len=PACKET_SIZE;

    msg_pool[id].msg_name=&(src_addr_pool[id]);
    msg_pool[id].msg_namelen=sizeof(src_addr_pool[id]);
    msg_pool[id].msg_iov=iov_pool[id];
    msg_pool[id].msg_iovlen=1;
    msg_pool[id].msg_control=0;
    msg_pool[id].msg_controllen=0;
}

void RTP_interface::stop() {
    LOG_MSG("RTP_interface::stop()")
    exit = true;
}

int RTP_interface::get_buffer_id(){
    while(free_buffer_index.empty());

    int id = free_buffer_index.front(); 
    free_buffer_index.pop();
    return id; 
}

void RTP_interface::release_buffer(int id){
    free_buffer_index.push(id);
}

void RTP_interface::parse_packet(int id, int length){
    //1) get SSRC and CSRC from packet
    //2) find stream or create new stream
    //3) get payload
    //4) send signal to the stream for parsing

    LOG_MSG("RTP_interface::parse_packet()")

    SRTP::header* rtp_h = (SRTP::header*)buffer_pool[id];
    SRTP::fix_header(rtp_h);

    //printf("v:%u p:%u seq:%u\n", rtp_h->v, rtp_h->p, rtp_h->seq);

    BYTE* payload = nullptr;
    size_t header_size = SRTP::get_payload(rtp_h, buffer_pool[id], &payload);

    if(streams[rtp_h->ssrc] == NULL){
        streams[rtp_h->ssrc] = new SRTP_stream(SRTP_stream::DECODE);
    }
    p->parse_msg(payload, rtp_h, out_buffer_pool[id], 
                 streams[rtp_h->ssrc], id, length-header_size);
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
    LOG_MSG("RTP_interface::send()");

    sendto(rtp_sock, out_buffer_pool[id], size, 0, 
      (sockaddr*)&(src_addr_pool[id]), sizeof(src_addr_pool[id]));
    release_buffer(id);
}

//void set_parser(SRTP_parser *parser){
//    p = parser;
//}


void RTP_interface::operator()(){
    while(!exit){
        int id = get_buffer_id();
        int bytes = recvmsg(rtp_sock, &(msg_pool[id]), 0);
        if(bytes > 0){
            parse_packet(id, bytes);
        }
    }
}
