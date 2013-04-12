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
        char* encoding_name = 0;
        int (*transcode)(CBYTE* src, BYTE* dst, int l_src, int* l_dst, int pt);
        void (*to_raw)(CBYTE* src, BYTE* raw, int len_src, int* len_dst);
        void (*from_raw)(CBYTE* raw, BYTE* dst, int len_src, int* len_dst);
    };

    void init();
    void cleanup();
    int transcode(CBYTE* src, BYTE* dst, int len_src, int* len_dst, int pt_src, int pt_dst);
    void* get_transcode_function(int pt, int result, bool to_pcm=true);
}



#ifdef __cplusplus
}
#endif


#endif
