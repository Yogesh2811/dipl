#include "cl_util.h"
#include "rtp_interface.h"

#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    //initOpenCL();
    int err;

    RTP_interface  r(16000,16001,&err);
    boost::thread rtp(boost::ref(r));
    printf("\nerror - %d\n", err);


    getchar();
    r.stop();

    getchar();


    return 0;
}
