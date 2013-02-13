#include "rtp_interface.h"
#include "srtp_stream.h"
#include "srtp_parser.h"

#include <iostream>

using namespace std;

int RTP_interface::count = 0;

RTP_interface::RTP_interface(SRTP_parser* parser, int rtp, int rtcp, int* err){
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
    printf("RTP_interface::stop()\n");
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
    buffer_pool[id][length] = '\0';

    cout << "parse_packet(" << id << ", " << length << ")\n";
    
    p->decode_msg(buffer_pool[id], out_buffer_pool[id], id, length);
    //encode_srtp_callback(buffer_pool[id], out_buffer_pool[id], NULL, length);
    // print out info about the remote socket
    //char address[500];
    //inet_ntop(AF_INET6,&(src_addr_pool[id]),address,sizeof(src_addr_pool[id]));
    //int port = ntohs(src_addr_pool[id].sin6_port);
    //printf("%d from: %s port: %d\nmsg: %s\n\n\n",id,address,port,buffer_pool[id]);

    // 1) get SSRC and CSRC from packet
    // 2) find stream or create new stream
    // 3) send signal to the stream for parsing
    
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
    //
    // 2) release buffer id used for this packet
    // release_buffer(id);
    
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
