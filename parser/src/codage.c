#include "../include/codage.h"

#include "../include/bits.h"

extern int encode(unsigned char num, BitStream *dst){
    if (num < 2){
        pushbits(0, 3, dst);
        pushbits(num, 1, dst);
        return 4;
    }
    
    unsigned char e = 1;
    while ((1 << e) < num) e++;
    e--;
    //printf("for num %d exp2 is computed to be %d\n", num,e);

    unsigned char r = num - (1 << e);
    pushbits(e, 3, dst);
    pushbits(r, e, dst);
    return 3 + e;
}

extern unsigned char decode(BitStream *src){
    unsigned char e = readbits(src, 3);
    if (e == 0){
        return readbits(src, 1);
    }
    
    unsigned char r = readbits(src, e);
    return r + (1 << e);
}