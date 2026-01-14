#ifndef DIF_H
    #define DIF_H 1

    int pnmtodif(const char* pnminput, const char* difoutput, int verbose);

    int diftopnm(const char* difinput, const char* pnmoutput, int verbose);
#endif