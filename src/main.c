#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <differential.h>
#include <pnm.h>




/*void encode_decode_test(){
    for (int dif = -255; dif < 256; dif+=1){
        printf("Original dif: %d, encoded: %d, decoded: %d\n",
            dif,
            encode_dif(dif),
            decode_dif(encode_dif(dif)));
    }
}*/

int main(void){
    PNMImage *img = read_pnm(".\\tests\\boat.pgm");
    printf("Dimensions: %d %d\nMagic number: %s\n", img->width, img->height, img->magic);
    pgm_to_difference(img);
    write_pnm_image("boat_trace.pgm", img);
    differential_to_pnm(img);
    write_pnm_image("boat_decode.pgm", img);
    free(img);
    //encode_decode_test();
    return 0;
}