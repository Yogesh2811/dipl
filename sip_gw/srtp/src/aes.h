#ifndef AES_H
#define AES_H

#define ROUND_KEY_SIZE 11
#define ROUNDS 10
#define BLOCK_SIZE 16
#define ROWS 4
#define COLUMNS 4

typedef unsigned char BYTE;
typedef const unsigned char CBYTE;

namespace AES {
    void expand_key(CBYTE* master_key, BYTE round_key[ROUND_KEY_SIZE][BLOCK_SIZE]);
    void srtp_encode(CBYTE* src, BYTE* dst, CBYTE* key, CBYTE* iv, int length);
    void srtp_decode(CBYTE* src, BYTE* dst, CBYTE* key, CBYTE* iv, int length);
    void test();
    void print_state(CBYTE* state);
};

#endif
