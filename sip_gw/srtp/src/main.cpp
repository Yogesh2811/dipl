#include "cl_util.h"
#include "rtp_interface.h"
#include "srtp_parser.h"
#include "daemon.h"


int main(int argc, char* argv[]) {
    //initOpenCL();
    int err;

    SRTP_parser* p;
    RTP_interface* r;
    //Daemon* d;
    
    p = new SRTP_parser(r, SRTP_parser::SERIAL_EXECUTION);
    boost::thread srtp_pars(boost::ref(*p));

    r = new RTP_interface(p, 16000, 16001, &err);
    boost::thread rtp(boost::ref(*r));
    //printf("\nerror - %d\n\n\n", err);

    //r->set_parser(p);
    p->set_interface(r);
    
    //r->send(0,0);
    //p->decode_msg(NULL, NULL, 0, 0);

    //d = new Daemon(p,r);
    //boost::thread daemon(boost::ref(*d));

    getchar();
    r->stop();
    //p->stop();
    //d->stop();

    return 0;
}




//////////////////////////////////////////////////////////////
