#ifndef DAEMON_H
#define DAEMON_H

class SRTP_parser;
class RTP_interface;

class Daemon {
    public:
        Daemon(SRTP_parser *p, RTP_interface *r);
        ~Daemon();
        void operator()();
        void stop();
        void encode_callback();

    private:
        bool quit;
        SRTP_parser *p;
        RTP_interface *r;

};


#endif
