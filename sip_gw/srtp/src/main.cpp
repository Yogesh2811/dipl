#include "cl_util.h"
#include "rtp_interface.h"
#include "srtp_parser.h"
#include "daemon.h"
#include "aes.h"
#include "plugins.h"

int main(int argc, char* argv[]) {
    //initOpenCL();
    Plugins::init();
    /*int err;

    //AES::test();
    //GPU::test();
    SRTP_parser* p[PARSER_COUNT];
    RTP_interface* r;
    Daemon* d;
    
    p[0] = new SRTP_parser(r, SRTP_parser::SERIAL_EXECUTION);
    p[1] = new SRTP_parser(r, SRTP_parser::PARALEL_EXECUTION);
    boost::thread srtp_pars1(boost::ref(*p[0]));
    boost::thread srtp_pars2(boost::ref(*p[1]));

    d = new Daemon(r,p);
    boost::thread daemon(boost::ref(*d));
    
    r = new RTP_interface(d, 16000, 16001, &err);
    boost::thread rtp(boost::ref(*r));
    //printf("\nerror - %d\n\n\n", err);

    //r->set_parser(p);
    p[0]->set_interface(r);
    p[1]->set_interface(r);
    d->set_interface(r);

    //r->send(0,0);
    //p->decode_msg(NULL, NULL, 0, 0);


    getchar();
    r->stop();
    //p->stop();
    //d->stop();*/

    return 0;
}




//////////////////////////////////////////////////////////////
