#ifndef TRANSCODE_PLUGINS_H
#define TRANSCODE_PLUGINS_H

/*
 * Data types
 */
typedef unsigned char BYTE;
typedef const BYTE CBYTE;

/*
 * Codec baisic identification and information as defined in RFC3551
 */
extern const char* encoding_name;
extern const int PT;

/*
 * Transcodes multimedia data from one codec to another codec
 *
 * @param src - source data
 * @param dst - expects allocated buffer with big enough size
 * @param l_src - length of src data
 * @param d_src - sets length of dst data
 * @param pt - pt of codec to be transcoded into
 * @param id - stream identifier for buffers
 *
 * @returns 
 * 1  if succes 
 * 0  if fail (needs to be transcoded to raw and from raw) 
 * -1 if system unable to do this conversion even through raw format
 */
extern int (*transcode)(CBYTE* src, BYTE* dst, int l_src, int* l_dst, int pt, int id);

/*
 * Transcodes codec to raw PCM
 *
 * @param src - source data
 * @param dst - expects allocated buffer with big enough size
 * @param len_src - length of src data
 * @param len_dst - sets length of dst data
 * @param id - stream identifier for buffers
 */
extern void (*to_raw)(CBYTE* src, BYTE* raw, int len_src, int* len_dst, int id);

/*
 * Transcodes raw PCM to codec
 *
 * @param src - source data
 * @param dst - expects allocated buffer with big enough size
 * @param len_src - length of src data
 * @param len_dst - sets length of dst data
 * @param id - stream identifier for buffers
 */
extern void (*from_raw)(CBYTE* raw, BYTE* dst, int len_src, int* len_dst, int id);

#endif
