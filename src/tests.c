#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <differential.h>
#include <pnm.h>
#include <codage.h>
#include <bits.h>
#include <dif.h>


void test_differential(){
    PNMImage *img = read_pnm(".\\tests\\boat.pgm");
    printf("Dimensions: %d %d\nMagic number: P%d\n", img->width, img->height, img->magic);
    pnm_to_differential(img);
    write_pnm_image("boat_trace.pgm", img);
    differential_to_pnm(img);
    write_pnm_image("boat_decode.pgm", img);
    free(img);
}

void test_differential_rgb(){
    PNMImage *img = read_pnm(".\\tests\\gaspard.ppm");
    printf("Dimensions: %d %d\nMagic number: P%d\n", img->width, img->height, img->magic);
    pnm_to_differential(img);
    write_pnm_image("gaspard_trace.pgm", img);
    differential_to_pnm(img);
    write_pnm_image("gaspard_decode.pgm", img);
    free(img);
}

void test_byte_encode(){
    unsigned char bitlens[] = {2, 4, 5, 8};
    bit_lens(bitlens);

    unsigned char *buff = (unsigned char*)malloc(400);
    assert(buff != NULL);
    
    BitStream b;
    b.ptr = buff;
    b.cap = 8;
    b.off = 0;

    for(unsigned char i = 0; i < 255; i++){
        encode(i, &b);
        //printf("%d encoded at %p - ", i, b.ptr);
    }
    
    b.ptr = buff; b.off = 0; b.cap = 8;
    for(unsigned char i = 0; i < 255; i++)
        printf("\n%d", decode(&b));
    printf("\n");
    free(buff);
}

void end_to_end_pgm_test_gray(){
    printf("Encode: \n");
    pnmtodif("./tests/boat.pgm", "./boat.dif");

    printf("\nDecode: \n");
    diftopnm("./boat.dif", "./boat.pgm");
}

void end_to_end_pgm_test_rgb(){
    printf("Encode: \n");
    pnmtodif("./tests/gaspard.ppm", "./gaspard.dif");

    printf("\nDecode: \n");
    diftopnm("./gaspard.dif", "./gaspard.pgm");
}

int main(void){
    end_to_end_pgm_test_rgb();
    //test_byte_encode();
    //test_differential();
    //test_differential_rgb();
    //encode_decode_test();
    return 0;
}