#ifndef AES_H
#define AES_H

#define ROUND_KEY_SIZE 11
#define ROUNDS 10
#define BLOCK_SIZE 16
#define ROWS 4
#define COLUMNS 4

typedef unsigned char BYTE;

void srtp_encode(BYTE* src, BYTE* dst, BYTE* key, BYTE* counter, int length);
void srtp_decode(BYTE* src, BYTE* dst, BYTE* key, BYTE* counter, int length);

#endif
