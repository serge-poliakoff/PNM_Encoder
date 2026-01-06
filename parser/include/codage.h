#ifndef CODAGE_H
#define CODAGE_H 1452

#include <bits.h>

typedef struct{
    unsigned char value;
    size_t len;
} byte_value;

unsigned char* bit_lens(unsigned char* bitlens);

byte_value interval_code(int start_index);

int encode(unsigned char num, BitStream *dst);

unsigned char decode(BitStream *src);


#endif