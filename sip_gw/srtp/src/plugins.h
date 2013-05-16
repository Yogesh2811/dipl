/** @file */
#ifndef PLUGINS_H
#define PLUGINS_H

//#include <string>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char BYTE;
typedef const BYTE CBYTE;

/** \brief interface for plugins */
namespace Plugins{
    /** \brief structure for loaded codecs */
    struct Codec { 
        int PT;                    ///< Payload Type
        char* encoding_name = 0;   ///< Name of the codec
        /**
         * \brief Transcodes multimedia data from one codec to another codec
         *
         * @param src source data
         * @param dst expects allocated buffer with big enough size
         * @param l_src length of src data
         * @param d_src sets length of dst data
         * @param pt pt of codec to be transcoded into
         * @param id stream identifier for buffers
         *
         * @returns 
         * 1  if succes 
         * 0  if fail (needs to be transcoded to raw and from raw) 
         * -1 if system unable to do this conversion even through raw format
         */
        int (*transcode)(CBYTE* src, BYTE* dst, int l_src, int* l_dst, int pt, int id);
        /**
         * \brief Transcodes codec to raw PCM
         *
         * @param src source data
         * @param dst expects allocated buffer with big enough size
         * @param len_src length of src data
         * @param len_dst sets length of dst data
         * @param id stream identifier for buffers
         */
        void (*to_raw)(CBYTE* src, BYTE* raw, int len_src, int* len_dst, int id);
        /**
         * Transcodes raw PCM to codec
         *
         * @param src source data
         * @param dst expects allocated buffer with big enough size
         * @param len_src length of src data
         * @param len_dst sets length of dst data
         * @param id stream identifier for buffers
         */
        void (*from_raw)(CBYTE* raw, BYTE* dst, int len_src, int* len_dst, int id);
    };
    /** \brief initialization of dynamic loading and loading of the codecs */
    void init();
    /** \brief deallocation codecs */
    void cleanup();
    /** \brief interface transcoding function*/
    int transcode(BYTE* src, BYTE* dst, int l_src, int* l_dst, int pt_src, int pt_dst, int id);
}

#ifdef __cplusplus
}
#endif

#endif
/** @} */