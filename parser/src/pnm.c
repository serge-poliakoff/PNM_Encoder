#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <pnm.h>

PNMImage* read_pnm(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    assert(fp != NULL);

    PNMImage *img = malloc(sizeof(PNMImage));
    if (!img) {
        fclose(fp);
        return NULL;
    }

    // Read magic number
    if (fscanf(fp, "%2s", img->magic) != 1) {
        fprintf(stderr, "Image is not of pnm format, or is currupted:\nReading magic number failed");
        free(img);
        fclose(fp);
        return NULL;
    }
    img->magic[2] = '\0';

    // Skip comments and read width/height
    int c;
    do { c = fgetc(fp); } while (c == '\n' || c == '\r' || c == ' ' || c == '\t');
    ungetc(c, fp);

    while ((c = fgetc(fp)) == '#') {
        while ((c = fgetc(fp)) != '\n' && c != EOF);
    }
    ungetc(c, fp);
    
    if (fscanf(fp, "%d %d", &(img->width), &(img->height)) != 2) {
        fprintf(stderr, "Image is not of pnm format, or is currupted:\nReading magic number failed");
        free(img);
        fclose(fp);
        return NULL;
    }

    // For P5/P6, skip maxval
    if (strcmp(img->magic, "P5") == 0 || strcmp(img->magic, "P6") == 0) {
        int maxval;
        if (fscanf(fp, "%d", &maxval) != 1) { free(img); fclose(fp); return NULL; }
        // Skip single whitespace after maxval
        fgetc(fp);
    } else {
        // Skip single whitespace after header
        fgetc(fp);
    }

    // Calculate data size
    size_t size = 0;
    if (strcmp(img->magic, "P1") == 0 || strcmp(img->magic, "P2") == 0 || strcmp(img->magic, "P3") == 0) {
        // ASCII formats: not supported for binary read
        free(img); fclose(fp); return NULL;
    } else if (strcmp(img->magic, "P4") == 0) {
        // Bitmap (binary): 1 bit per pixel, padded to full bytes per row
        size = ((img->width + 7) / 8) * img->height;
    } else if (strcmp(img->magic, "P5") == 0) {
        // Grayscale (binary): 1 byte per pixel
        size = img->width * img->height;
    } else if (strcmp(img->magic, "P6") == 0) {
        // RGB (binary): 3 bytes per pixel
        size = img->width * img->height * 3;
    } else {
        free(img); fclose(fp); return NULL;
    }

    img->data = malloc(size);
    if (!img->data) { free(img); fclose(fp); return NULL; }
    img->data_size = size;


    
    if (fread(img->data, 1, size, fp) != size) {
        free(img->data); free(img); fclose(fp); return NULL;
    }
    
    fclose(fp);
    return img;
}

void write_pnm_image(const char* filename, PNMImage* img){
    FILE *fp = fopen(filename, "wb");
    assert(fp != NULL);

    fprintf(fp, "%s\n", img->magic);
    fprintf(fp,"%d %d %d\n", img->width, img->height, 255);
    fwrite(img->data, 1, img->data_size, fp);
    
    fclose(fp);
}