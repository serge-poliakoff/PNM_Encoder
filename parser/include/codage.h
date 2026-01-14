#ifndef CODAGE_H
#define CODAGE_H 1452

#include <bits.h>



unsigned char* bit_lens(unsigned char* bitlens);

int encode(unsigned char num, BitStream *dst);

unsigned char decode(BitStream *src);

int codage_verbosity(int v);

#endif