#ifndef BITS_H
    #define BITS_H 1452

    #include <stdio.h>
    typedef struct
    { 
        unsigned char* ptr; /* pointeur sur l’octet courant */
        size_t cap; /* capacité en écriture */
        size_t off; /* offset en lecture */
    } BitStream;

    unsigned char extraireBit(unsigned char src, int n);

    unsigned char setBit(unsigned char src, int n, unsigned char value);

    void writeBits(unsigned char src);

    void pushbits(unsigned char src, size_t nbit, BitStream* dst);

    int readbits(BitStream* src, size_t n);
#endif