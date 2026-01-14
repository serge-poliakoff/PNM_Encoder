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

static void rgb_to_dif(unsigned char *data, size_t data_size){
    unsigned char prev_brut[3] = {data[0], data[1], data[2]};
    for(size_t i = 3; i < data_size; i++){
        int dif = (int)data[i] - (int)prev_brut[i % 3];
        dif /= 2;

        unsigned char y = encode_dif(dif * 2);
        prev_brut[i % 3] = prev_brut[i % 3] + dif * 2;
        data[i] = y;
    }
}

static void grayscale_to_dif(unsigned char *data, size_t data_size){
    unsigned char prev_brut = data[0];
    for(size_t i = 1; i < data_size; i++){
        int dif = (int)data[i] - (int)prev_brut;
        dif /= 2;

        unsigned char y = encode_dif(dif * 2);
        prev_brut = prev_brut + dif * 2;
        data[i] = y;
    }
}

static void differential_to_rgb(unsigned char *data, size_t data_size){
    for(size_t i = 3; i < data_size; i++){
        unsigned char y = data[i];
        int x = decode_dif(y);
        if ((data[i-3] + x) > 255){
            data[i] = 255;
            //printf("Warning: difference value overflow: %d\n", x);
        }else
            data[i] = data[i-3] + x;
    }
}

static void differential_to_grayscale(unsigned char* data, size_t data_size){
    for(size_t i = 1; i < data_size; i++){
        unsigned char y = data[i];
        int x = decode_dif(y);
        if ((data[i-1] + x) > 255){
            data[i] = 255;
            //printf("Warning: difference value overflow: %d\n", x);
        }else{
            data[i] = data[i-1] + x;
        }
    }
}

extern void pnm_to_differential(PNMImage* img){
    if (img -> magic == 6){
        rgb_to_dif(img->data, img->data_size);
    }else{
        grayscale_to_dif(img->data, img->data_size);
    }
}

extern void differential_to_pnm(PNMImage* img){
    if (img -> magic == 6){
        differential_to_rgb(img->data, img->data_size);
    }else{
        differential_to_grayscale(img->data, img->data_size);
    }
}
