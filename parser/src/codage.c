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
//unsigned char intervals[] = {0, 4, 20, 52, 255};
//unsigned char lengths_exp[] = {2, 4, 5, 8};  //length in bytes of number encoded in corresponding interval
static void clean_up();


extern unsigned char* bit_lens(unsigned char* bitlens){
    static unsigned char* length = NULL;
    if (length == NULL){
        assert(bitlens != NULL);

        length = (unsigned char*)malloc(4);
        assert(length != NULL);

        for(int i = 0; i < 4; i++) length[i] = bitlens[i];

        assert(atexit(clean_up) == 0);
    }
    return length;
}

static unsigned char* intervals(){
    static unsigned char ints[5], initialised = 0;
    if (initialised == 0){
        unsigned char* lengths = bit_lens(NULL);
        assert(lengths != NULL);
        printf("Setting up intervals for encode/decode process\nLength:\n");
        for(int i = 0; i < 4; i++)
            printf("%d\t", lengths[i]);

        ints[0] = 0;
        printf("\nIntervals: %d\t", ints[0]);
        for(int i = 1; i < 4; i++){
            ints[i] = ints[i - 1] + (1 << lengths[i - 1]);
            printf("%d\t", ints[i]);
        }
        ints[4] = 255;
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

static void clean_up(){
    print_setts();
    free(bit_lens(NULL));
    //free(intervals());
}

/*typedef struct{
    unsigned char value;
    size_t len;
} byte_value;*/

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