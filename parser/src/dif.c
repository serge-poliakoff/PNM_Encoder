#include <stdlib.h>
#include <stdio.h>


#include <pnm.h>
#include <codage.h>
#include <differential.h>
#include <bits.h>

unsigned char standart_bitlen[] = {2, 4, 5, 8};
unsigned short magic_grayscale = 0xD1FF;
unsigned short magic_rgb = 0xD3FF;

//to do: standartize error gestion

extern int pnmtodif(const char* pnminput, const char* difoutput, int verbose){
    printf("\nCompressing file: %s -> %s...\n", pnminput, difoutput);
    PNMImage* pnm = read_pnm(pnminput);
    if (pnm == NULL){
        fprintf(stderr, "Opening %s went wrong...\nExiting application", pnminput);
        return 1;
    }
    if(verbose)
        printf("Dimensions: %d x %d; magic -> P%d\n", pnm->width, pnm->height, pnm->magic);
    
    pnm_to_differential(pnm);
    
    //encode data to buffer
    unsigned char *dif_data = (unsigned char*)malloc(pnm->data_size * 1.35);
    if (dif_data == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        free(pnm->data);
        free(pnm);
        exit(1);
    }
    BitStream b;
    b.ptr = &(dif_data[1]); b.cap = 8; b.off = 0;
    codage_verbosity(verbose);
    bit_lens(standart_bitlen);
    size_t dif_data_size = 0;
    size_t first_pixel = pnm->magic == 5 ? 1 : 3;
    for(size_t i = first_pixel; i < pnm->data_size; i++){
        dif_data_size += encode(pnm->data[i], &b);
    }
    dif_data_size /= 8; //encode return number of bits encoded
    //first pixel without any changes
    dif_data[0] = pnm->data[0];
    if (pnm->magic == 6){
        dif_data[1] = pnm->data[1];
        dif_data[2] = pnm->data[2];
    }
    dif_data_size += 2; // (one for first byte and second for flooring it to lines higher)
    if(verbose)
        printf("Size of data encoded in bytes: %lld\n", dif_data_size);

    FILE *outp = fopen(difoutput, "wb");
    fwrite(pnm->magic==5?&magic_grayscale:&magic_rgb, 2, 1, outp);
    fwrite(&(pnm->width), 2, 1, outp); fwrite(&(pnm->height), 2, 1, outp);
    unsigned char q = 4;
    fwrite(&q, 1, 1, outp); //encoding quantificator
    for(int i = 0; i < 4; i++) fwrite(&standart_bitlen[i], 1, 1, outp);
    fwrite(dif_data, 1, dif_data_size, outp);

    double compression = (double)dif_data_size / (double)(pnm->data_size);
    printf("Compression tax for %s made up %f%c\n",pnminput, compression * 100, '%');
    fclose(outp);
    free(dif_data);
    free(pnm->data); 
    free(pnm);
    
    return 0;
}


extern int diftopnm(const char* difinput, const char* pnmoutput, int verbose){
    FILE *dif = fopen(difinput, "rb");
    if (dif == NULL){
        fprintf(stderr,"Error: cannot open file %s\n",difinput);
        return 1;
    }

    unsigned short magic;
    fread(&magic, 2, 1, dif);
    if (magic != magic_grayscale && magic != magic_rgb){
        fprintf(stderr,"Error processing file %s: unknown magic number\n", difinput);
        fclose(dif);
        return 1;
    }
    char pnm_magic = magic == magic_grayscale ? 5 : 6;
    unsigned short width, height;
    fread(&width, 2, 1, dif); fread(&height, 2, 1, dif);
    size_t data_size = width * height * (pnm_magic == 5 ? 1 : 3);
    if(verbose)
        printf("Width: %d, Height: %d, Magic: P%d\n", width, height, pnm_magic);

    //reading & configuring encoding
    fread(&magic,1,1,dif); //skipping q = 4
    unsigned char bitlens[] = {0,0,0,0};
    if(fread(&bitlens, 1, 4, dif) != 4){
        fprintf(stderr,"Error processing file %s: cannot read encoding\n", difinput);
        fclose(dif);
        return 1;
    }
    codage_verbosity(verbose);
    bit_lens(bitlens);

    //reading pixels (first & all the rest encoded)
    if(verbose)
        printf("Reading pixels...\n");
    unsigned char *buf = (unsigned char*)malloc(data_size*1.35);
    if (buf == NULL){
        fprintf(stderr, "Error: memory allocation failed\n");
        fclose(dif);
        exit(1);
    }

    //have to read pixel by pixel as we don't know the final size of encrypted image
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
    if(verbose)
        printf("Decoding: File processing finished successfully\n");

    BitStream b;
    size_t first_pixel = pnm_magic == 5 ? 1 : 3;
    b.ptr = &(buf[first_pixel]); b.off = 0; b.cap = 8;

    //final pnm image
    PNMImage pnm; pnm.width = width; pnm.height = height; pnm.magic = pnm_magic;
    pnm.data_size = data_size;
    pnm.data = (unsigned char*)malloc(pnm.data_size);
    if (pnm.data == NULL){
        fprintf(stderr, "Error: memory allocation failed\n");
        free(buf);
        exit(1);
    }

    pnm.data[0] = buf[0];
    if(pnm.magic == 6){
        pnm.data[1] = buf[2];
        pnm.data[1] = buf[2];
    }
    for(size_t i = first_pixel; i < pnm.data_size; i++){
        pnm.data[i] = decode(&b);
    }

    differential_to_pnm(&pnm);
    write_pnm_image(pnmoutput, &pnm);

    free(buf);
    free(pnm.data);
    return 0;
}