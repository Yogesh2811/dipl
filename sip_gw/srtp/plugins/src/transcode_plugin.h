#ifndef TRANSCODE_PLUGINS_H
#define TRANSCODE_PLUGINS_H

/*
 * Codec baisic identification and information as defined in RFC3551
 */
extern const char* encoding_name;
extern const int PT;

/*
 * Transcodes multimedia data from one codec to another codec
 *
 * @param src - source data
 * @param dst - expects NULL, if fails or unable to convert, sets to NULL as well
 * @param length - length of src data
 * @param codec - codec to be transcoded into
 *
 * @returns 
 * 1  if succes 
 * 0  if fail (needs to be transcoded to raw and from raw) 
 * -1 if system unable to do this conversion even through raw format
 */
extern int (*transcode)(const unsigned char* src, 
              unsigned char* dst, 
              int len_src, 
              int* len_dst, 
              int codec);

/*
 * Transcodes codec to raw LPC
 *
 * @param src - source data
 * @param dst - expects NULL, if fails or unable to convert, sets to NULL as well
 * @param length - length of src data
 */
void to_raw(const unsigned char* src, unsigned char* raw, int len_src, int* len_dst);

/*
 * Transcodes raw LPC to codec
 *
 * @param src - source data
 * @param dst - expects NULL, if fails or unable to convert, sets to NULL as well
 * @param length - length of src data
 */
void from_raw(const unsigned char* raw, unsigned char* dst, int len_src, int* len_dst);

#endif
