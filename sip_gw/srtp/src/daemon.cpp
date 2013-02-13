#include "daemon.h"
#include "rtp_interface.h"
#include "srtp_parser.h"

#include <vector>
#include <iostream>

using namespace std;

void Daemon::encode_callback(){
    cout << "callback\n";
    //p->encode_msg(src, dst, stream, length);
}


Daemon::Daemon(SRTP_parser *parser, RTP_interface *rtp){
    quit = false;
    p = parser;
    r = rtp;
}

Daemon::~Daemon(){
}

void Daemon::operator()(){
    while(!quit);
}

void Daemon::stop(){
    cout << "Daemon::stop()\n";
    quit = true;
}
