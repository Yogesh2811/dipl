#ifndef AES_H
#define AES_H

#define ROUND_KEY_SIZE 11
#define ROUNDS 10
#define BLOCK_SIZE 16
#define ROWS 4
#define COLUMNS 4

typedef unsigned char BYTE;

int srtp_encode(BYTE* src, BYTE* dst, BYTE* key, int length);
int srtp_decode(BYTE* src, BYTE* dst, BYTE* key, int length);
void test();

#endif
