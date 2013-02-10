#include "cl_util.h"
#include "rtp_interface.h"
#include "srtp_parser.h"

#include <vector>
#include <iostream>

using namespace std;

void srtp_parse();

int main(int argc, char* argv[]) {
    //initOpenCL();
    int err;

    SRTP_parser p(SRTP_parser::SERIAL_EXECUTION);
    boost::thread srtp_pars(boost::ref(p));

    RTP_interface r(16000,16001,&err);
    boost::thread rtp(boost::ref(r));
    printf("\nerror - %d\n\n\n", err);


    getchar();
    r.stop();

    getchar();


    return 0;
}




//////////////////////////////////////////////////////////////
void srtp_parse()
