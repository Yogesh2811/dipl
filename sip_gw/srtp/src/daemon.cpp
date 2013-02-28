#include "daemon.h"
#include "rtp_interface.h"
#include "srtp_parser.h"
#include "srtp_stream.h"
#include "utils.h"


Daemon::Daemon(RTP_interface *rtp, SRTP_parser *parsers[PARSER_COUNT]){
    quit = false;
    r = rtp;
    parser = parsers;
}

Daemon::~Daemon(){
}

void Daemon::operator()(){
    while(!quit);
}

void Daemon::stop(){
    LOG_MSG("Daemon::stop()")
    quit = true;
}


void Daemon::parse_msg(const BYTE* in, SRTP::header *h, BYTE* out, SRTP_stream* s,
                       int id, int length){
    //select the best parser
    LOG_MSG("Daemon::parse_msg()")
    parser[0]->parse_msg(in, h, out, s, id, length);
}


void Daemon::set_interface(RTP_interface* i){
    r = i;
}
