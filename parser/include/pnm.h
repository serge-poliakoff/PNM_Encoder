#ifndef PNM_H
    #define PNM_H 1
    #include <stdlib.h>

    typedef struct {
        char magic[3];   // "P1" to "P6", null-terminated
        int width;
        int height;
        unsigned char *data;
        size_t data_size;
    } PNMImage;

    PNMImage* read_pnm(const char *filename);

    void write_pnm_image(const char* filename, PNMImage* img);
#endif