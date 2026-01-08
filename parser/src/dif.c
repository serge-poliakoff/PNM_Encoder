#include <stdlib.h>
#include <stdio.h>


#include <pnm.h>
#include <codage.h>
#include <differential.h>
#include <bits.h>

unsigned char standart_bitlen[] = {2, 4, 5, 8};
unsigned short magic_grayscale = 0xD1FF;

extern int pnmtodif(const char* pnminput, const char* difoutput){
    PNMImage* pnm = read_pnm(pnminput);
    if (pnm == NULL){
        fprintf(stderr, "Opening %s went wrong...\nExiting application", pnminput);
        return 1;
    }
    printf("Dimensions: %d x %d; magic -> P%d\n", pnm->width, pnm->height, pnm->magic);

    int res_diff = pgm_to_difference(pnm);
    if (res_diff != 0){
        fprintf(stderr, "An error occured while processing %s...\nExiting application", pnminput);
        free(pnm->data); free(pnm);
        return 1;
    }

    //why bother with another PNMImage ??
    PNMImage *dif = (PNMImage*)malloc(sizeof(PNMImage));
    dif->magic = pnm->magic;
    dif->width = pnm->width; dif->height = pnm->height;
    dif->data = (unsigned char*)malloc(pnm->data_size * 1.35);
    //encode
    BitStream b;
    b.ptr = &(dif->data[1]); b.cap = 8; b.off = 0;
    bit_lens(standart_bitlen);
    dif->data_size = 0;
    for(size_t i = 1; i < pnm->data_size; i++){
        dif->data_size += encode(pnm->data[i], &b);
    }
    dif->data_size /= 8;
    dif->data[0] = pnm->data[0]; //first pixel without any changes
    dif->data_size += 2; // (one for first byte and second for flooring it to lines higher)
    printf("Size of data encoded in bytes: %d", dif->data_size);

    FILE *outp = fopen(difoutput, "wb");
    fwrite(&magic_grayscale, 2, 1, outp); //magic number 0xD3FF in case of rgb
    fwrite(&(dif->width), 2, 1, outp); fwrite(&(dif->height), 2, 1, outp);
    unsigned char q = 4;
    fwrite(&q, 1, 1, outp); //encoding quant'
    for(int i = 0; i < 4; i++) fwrite(&standart_bitlen[i], 1, 1, outp);
    fwrite(dif->data, 1, dif->data_size, outp);

    fclose(outp);
    free(dif->data);    free(dif);
    free(pnm->data);    free(pnm);
    
    return 0;
}


extern int diftopnm(const char* difinput, const char* pnmoutput){
    FILE *dif = fopen(difinput, "rb");
    if (dif == NULL){
        fprintf(stderr,"Error: cannot open file %s\n",difinput);
        return 1;
    }

    unsigned short magic;
    fread(&magic, 2, 1, dif);
    if (magic != magic_grayscale){
        fprintf(stderr,"Error processing file %s: unknown magic number\n", difinput);
        fclose(dif);
        return 1;
    }
    unsigned short width, height;
    fread(&width, 2, 1, dif); fread(&height, 2, 1, dif);
    printf("Width: %d, Height: %d\n", width, height);

    //reading & configuring encoding
    fread(&magic,1,1,dif); //skipping q = 4
    unsigned char bitlens[] = {0,0,0,0};
    if(fread(&bitlens, 1, 4, dif) != 4){
        fprintf(stderr,"Error processing file %s: cannot read encoding\n", difinput);
        fclose(dif);
        return 1;
    }
    bit_lens(bitlens);

    //reading pixels (first & all the rest encoded)
    unsigned char *buf = (unsigned char*)malloc(width*height*1.35);
    if (fread(buf, 1, 1, dif) != 1){
        fprintf(stderr,"Error processing file %s: empty body\n", difinput);
        free(buf);
        fclose(dif);
        return 1;
    }
    for(size_t i = 1;;i++){
        int res = fread(&(buf[i]), 1, 1, dif);
        if (res == 0) break;
    }
    fclose(dif);
    BitStream b;
    b.ptr = &(buf[1]); b.off = 0; b.cap = 8;

    //final pnm image
    PNMImage pnm; pnm.width = width; pnm.height = height; pnm.magic = 5;
    pnm.data = (unsigned char*)malloc(width*height/**layers*/);
    pnm.data[0] = buf[0];
    for(size_t i = 1; i < width * height; i++){
        pnm.data[i] = decode(&b);
    }
    pnm.data_size = width * height;

    differential_to_pnm(&pnm);
    write_pnm_image(pnmoutput, &pnm);

    free(buf);
    free(pnm.data);
    return 0;
}