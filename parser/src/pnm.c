#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <pnm.h>

extern PNMImage* read_pnm(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL){
        fprintf(stderr, "Error opening file %s:\nFile does not exist", filename);
        return NULL;
    }

    PNMImage *img = malloc(sizeof(PNMImage));
    if (img == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(1);
    }

    // read magic number
    if (fscanf(fp, "P%d", &(img->magic)) != 1) {
        fprintf(stderr, "Image is not of pnm format, or is currupted:\nReading magic number failed\n");
        free(img); fclose(fp);
        return NULL;
    }
    // working only on binary data
    if (img->magic < 5 || img->magic > 6){
        fprintf(stderr, "This programm work only with binary grayscale/rgb pnm images\n");
        free(img); fclose(fp);
        return NULL;
    }

    // skip comments and read width/height
    int c;
    do { c = fgetc(fp); } while (c == '\n' || c == '\r' || c == ' ' || c == '\t');
    ungetc(c, fp);

    while ((c = fgetc(fp)) == '#') {
        while ((c = fgetc(fp)) != '\n' && c != EOF);
    }
    ungetc(c, fp);
    
    if (fscanf(fp, "%d %d", &(img->width), &(img->height)) != 2) {
        fprintf(stderr, "Image is not of pnm format, or is currupted:\nReading dimensions number failed\n");
        free(img); fclose(fp);
        return NULL;
    }

    // skipping maxvalue as it can't mentioned in .dif format by its specification
    int maxval;
    if (fscanf(fp, "%d", &maxval) != 1) { 
        fprintf(stderr, "Image is not of pnm format, or is currupted:\nReading maximum value failed\n");
        free(img); fclose(fp);
        return NULL;
     }
    fgetc(fp);

    // Calculate data size
    size_t size = img->width * img->height * 
        (img->magic == 5 ? 1 : 3);

    img->data = malloc(size);
    if (img->data == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        free(img);
        exit(1);
    }

    img->data_size = size;
    if (fread(img->data, 1, size, fp) != size) {
        fprintf(stderr, "Error: reading pixels from fail failed.\n");
        free(img->data); free(img); fclose(fp);
        return NULL;
    }
    
    fclose(fp);
    return img;
}


//todo: make int return error code
extern void write_pnm_image(const char* filename, PNMImage* img){
    FILE *fp = fopen(filename, "wb");
    assert(fp != NULL);

    fprintf(fp, "P%d\n", img->magic);
    fprintf(fp,"%d %d %d\n", img->width, img->height, 255);
    fwrite(img->data, 1, img->data_size, fp);
    
    fclose(fp);
}