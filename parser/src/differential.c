#include <stdio.h>
#include <stdlib.h>

#include <differential.h>


static unsigned char encode_dif(int dif){
    dif /= 2;
    if (dif > 127 || dif < -127) printf("Difference encode overflow");
    unsigned char y;
    if (dif >= 0) y = (unsigned char)(dif * 2);
    else y = (unsigned char)(-2 * dif - 1);
    return y;
}

static int decode_dif(unsigned char y){
    int x;
    if (y % 2 == 1) x = -1 * ((int)y + 1) / 2;
    else x = (int)y / 2;
    x *= 2;
    return x;
}


extern int pgm_to_difference(PNMImage* img){
    unsigned char prev_brut = img->data[0];
    for(size_t i = 1; i < img->data_size; i++){
        int raw_dif = (int)img->data[i] - (int)prev_brut;
        int dif = raw_dif / 2;

        // Clip dif so that decoded value stays in 0..255
        int min_dif = (-prev_brut + 1) / 2; // ceil
        int max_dif = (255 - prev_brut) / 2; // floor
        if (dif < min_dif) dif = min_dif;
        if (dif > max_dif) dif = max_dif;

        unsigned char y = encode_dif(dif * 2); // encode the clipped difference (multiplied back)
        prev_brut = prev_brut + dif * 2; // simulate what decoder will do
        img->data[i] = y;
    }
    return 0;
}


extern int differential_to_pnm(PNMImage* img){
    for(size_t i = 1; i < img->data_size; i++){
        unsigned char y = img->data[i];
        int x = decode_dif(y);
        if ((img->data[i-1] + x) > 255){
            img->data[i] = 255;
            printf("Difference value overflow: %d\n", x);
        }else
            img->data[i] = img->data[i-1] + x;
    }
    return 0;
}
