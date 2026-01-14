#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../include/codage.h"

#include "../include/bits.h"

typedef struct{
    unsigned char value;
    size_t len;
} byte_value;

int num_inters = 4;

extern int codage_verbosity(int v){
    static int verbose, initialized = 0;
    if (!initialized){
        verbose = v;
        initialized = 1;
    }

    return verbose;
}

extern unsigned char* bit_lens(unsigned char* bitlens){
    static unsigned char length[4], initialized = 0;
    if (initialized == 0){
        assert(bitlens != NULL);

        for(int i = 0; i < 4; i++) length[i] = bitlens[i];

        initialized = 1;
    }
    return length;
}

static unsigned char* intervals(){
    static unsigned char ints[5], initialised = 0;
    if (initialised == 0){
        unsigned char* lengths = bit_lens(NULL);
        if (lengths == NULL){
            fprintf(stderr, "Error: internal encoding algorithmes error");
            exit(1);
        }
        if (codage_verbosity(0)){
            printf("Setting up intervals for encode/decode process\nLength:\n");
            for(int i = 0; i < 4; i++)
                printf("%d\t", lengths[i]);
        }
        ints[0] = 0;
        if (codage_verbosity(0))
            printf("\nIntervals: %d\t", ints[0]);
        for(int i = 1; i < 4; i++){
            ints[i] = ints[i - 1] + (1 << lengths[i - 1]);
            if (codage_verbosity(0))
                printf("%d\t", ints[i]);
        }
        ints[4] = 255;
        if (codage_verbosity(0))
            printf("%d\n", ints[4]);
        initialised = 1;
    }
    return ints;
}

static void print_setts(){
    printf("Parameters of encoding calculated as:\nLengths: ");
    for(int i = 0; i < 4; i++)
        printf("%d\t", bit_lens(NULL)[i]);
    printf("\nIntervals: ");
    for(int i = 0; i < 5; i++)
        printf("%d - ", intervals()[i]);
}

extern byte_value interval_code(int start_index){
    byte_value res;
    if (intervals()[start_index + 1] == 255){
        res.value = (1 << (num_inters - 1)) - 1;
        res.len = num_inters - 1;
        return res;
    }
    res.value = 0;
    res.len = 1;
    while (start_index > 0)
    {
        res.value += 1 << (start_index - 1);
        res.len ++;
        start_index --;
    }
    return res;
}

extern int encode(unsigned char num, BitStream *dst){
    //find corresponding interval
    unsigned char e = 1;
    while ((intervals()[e] <= num) && (e < num_inters)) e++;
    e--;
    /*printf("number %d is computed to be in inteval (%d)%d to %d\n", num, e,
        intervals()[e], intervals()[e + 1]);*/
    
    byte_value inter_code = interval_code(e);
    unsigned char r = num - intervals()[e];

    pushbits(inter_code.value, inter_code.len, dst);
    pushbits(r, bit_lens(NULL)[e], dst);

    int total_length = bit_lens(NULL)[e] + inter_code.len;
    return total_length;
}

extern unsigned char decode(BitStream *src){
    int interval_ind = 0;
    unsigned char e = readbits(src, 1);
    while (e != 0 && ++interval_ind < (num_inters - 1))
    {
        e = readbits(src, 1);
    }
    //printf("Number found in interval %d to %d", intervals()[interval_ind], intervals()[interval_ind + 1]);
    
    unsigned char r = readbits(src, bit_lens(NULL)[interval_ind]);
    return intervals()[interval_ind] + r;
}