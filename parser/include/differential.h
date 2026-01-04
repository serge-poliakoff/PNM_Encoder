#ifndef DIFFERENTIAL_H
    #define DIFFERENTIAL_H 1

    #include <pnm.h>
    
    int pgm_to_difference(PNMImage* img);

    int differential_to_pnm(PNMImage* img);

#endif