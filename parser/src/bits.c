#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../include/bits.h"

unsigned char extraireBit(unsigned char src, int n){
    return (src >> n) & 1;
}

unsigned char setBit(unsigned char src, int n, unsigned char value){
    unsigned char init_v = extraireBit(src, n);
    if (init_v == value) return src;
    if (value > init_v) return src + (1 << n);
    else return src - (1 << n);
}

//for test purposes
void writeBits(unsigned char src){
    for(int i = 0; i < sizeof(unsigned char) * 8; i++){
        printf("%d", extraireBit(src, i));
    }
    printf("\t");
}

int readbits(BitStream* src, size_t n){
    assert(src != NULL);

    int res = 0;
    for (size_t i = 0; i < n; i ++){
        res += extraireBit(*src->ptr, src->off) << i;
        src->off++;
        if(src->off == 8){
            src->ptr ++;
            src->off = 0;
        }
    }

    return res;
}

void pushbits(unsigned char src, size_t nbit, BitStream* dst){
    assert(dst != NULL);
    
    int i = 0;
    while(dst->cap > 0){
        *dst->ptr = setBit(*dst->ptr, 8 - dst->cap, extraireBit(src, i));
        dst->cap--; i ++;
        if (i == nbit) return;
    }
    if (i < nbit){
        dst->ptr++;
        dst->cap = 8;
        while(i < nbit){
            *dst->ptr = setBit(*dst->ptr, 8 - dst->cap, extraireBit(src, i));
            dst->cap--; i ++;
        }
    }
}