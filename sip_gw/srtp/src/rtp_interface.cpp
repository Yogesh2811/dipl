#include "rtp_interface.h"

int RTP_interface::count = 0;

RTP_interface::RTP_interface(int rtp, int rtcp, int* err){
    err = 0;
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
    printf("\nstop\n");
    exit = true;
}

int RTP_interface::get_buffer_id(){
    int id = free_buffer_index.front(); 
    free_buffer_index.pop();
    return id; 
}

void RTP_interface::release_buffer(int id){
    free_buffer_index.push(id);
}

void RTP_interface::parse_packet(int id, int length){
    buffer_pool[id][length] = '\0';

    int port = ntohs(src_addr_pool[id].sin6_port);
    char* addr = "::1";
    printf("from: %s:%d\nmsg: %s\n\n\n",addr,port,buffer_pool[id]);
}

void RTP_interface::operator()(){
    while(!exit){
        printf("%d recv rtp\n", id);
        int id = get_buffer_id();
        int bytes = recvmsg(rtp_sock, &(msg_pool[id]), 0);
        if(bytes > 0){
            parse_packet(id, bytes);
        }
        release_buffer(id);
    }
}
