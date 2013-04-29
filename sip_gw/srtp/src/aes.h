#ifndef AES_H
#define AES_H

#define ROUND_KEY_SIZE 11
#define ROUNDS 10
#define BLOCK_SIZE 16
#define ROWS 4
#define COLUMNS 4

typedef unsigned char BYTE;

namespace AES {
    void expand_key(BYTE* master_key, BYTE round_key[ROUND_KEY_SIZE][BLOCK_SIZE]);
    void srtp_encode(BYTE* src, BYTE* dst, BYTE* key, BYTE* iv, int length);
    void srtp_decode(BYTE* src, BYTE* dst, BYTE* key, BYTE* iv, int length);
    void test();
    void print_state(BYTE* state);
};

#endif
