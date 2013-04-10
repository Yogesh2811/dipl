#ifndef PLUGINS_H
#define PLUGINS_H

//#include <string>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char BYTE;
typedef const BYTE CBYTE;

namespace Plugins{

    struct Codec {
        int PT;
        //std::string encoding_name;
        int (*transcode)(CBYTE* src, BYTE* dst, int len_src, int* len_dst, int codec);
        void (*to_raw)(CBYTE* src, BYTE* raw, int len_src, int* len_dst);
        void (*from_raw)(CBYTE* raw, BYTE* dst, int len_src, int* len_dst);
    };

    void init();
    void cleanup();
}



#ifdef __cplusplus
}
#endif


#endif
