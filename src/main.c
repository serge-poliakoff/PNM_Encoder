#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <pnm.h>
#include <dif.h>
#include <getopt.h>

static int compress = 0;
static int decompress = 0;
static int help = 0;

int compress_file(const char* filename){
    if (filename == NULL){
        fprintf(stdout, "Error: empty filename encountered while trying to compress file\n");
        return 1;
    }
    int filename_length = strlen(filename);
    char suffix[] = ".dif";

    char *compressed_name = (char*)malloc(filename_length + 4);
    if (compressed_name == NULL){
        fprintf(stdout, "Error: memory allocation failed\n");
        exit(1);
    }
    sprintf(compressed_name, "%s%s", filename, suffix);
    
    int result = pnmtodif(filename, compressed_name);
    if (result != 0){
        printf("Unfortunately, compressing %s failed. Check stderr to see error logs.\n", filename);
        return 1;
    }

    free(compressed_name);
    return 0;
}

int decompress_file(const char* filename){
    if (filename == NULL){
        fprintf(stderr, "Error: empty filename encountered while trying to decompress file\n");
        return 1;
    }
    int filename_length = strlen(filename);
    char suffix[] = ".pnm";

    char *compressed_name = (char*)malloc(filename_length + 4);
    if (compressed_name == NULL){
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(1);
    }
    
    sprintf(compressed_name, "%s%s", filename, suffix);
    
    int result = diftopnm(filename, compressed_name);
    if (result != 0){
        printf("Unfortunately, decompressing %s failed. Check stderr to see error logs.\n", filename);
        return 1;
    }
    
    free(compressed_name);
    return 0;
}

int main(int argc, char **argv){
  int c = 1;

  while (c >= 0)
    {
      static struct option long_options[] =
        {
          /* These options set a flag. */
          {"compress", no_argument, 0, 'c'},
          {"decompress",   no_argument, 0, 'd'},
          /* These options don’t set a flag.
             We distinguish them by their indices. */
          {"help",     no_argument,       0, 'h'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "cdh",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;

        case 'c':
          compress = 1;
          //printf("Compressing files option chosen\n");
          c = -1;
          break;

        case 'd':
          decompress = 1;
          //printf("Decompressing files option chosen\n");
          c = -1;
          break;

        case 'h':
          help = 1;
          //printf("Help option chosen\n");
          c = -1;
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
            printf("Error parsing arguments");
          abort ();
        }
    }

  /* Instead of reporting ‘--verbose’
     and ‘--brief’ as they are encountered,
     we report the final status resulting from them. */
  if (help){
    printf("Usage:\n");
    printf("\t-c (--compress) file [file2 etc.]\tfor compressing pnm images\n");
    printf("\t-d (--decompress) file [file2 etc.]\tfor decompressing pnm images\n");
    printf("\t-h (--help)\t\t\t\tto get help\n");

    printf("Note that this programm can compress only bynary pnm images in formats of P5 and P6\n");
    return 0;
  } 

  //at this point we are sure that our job is compressing/decompressing images
  if (optind < argc)
    {
      int treatedCount = 0;
      const int totalCount = argc - optind;
      while (optind < argc){
        int res;

        if (compress){
            res = compress_file(argv[optind++]);
        }else{
            res = decompress_file(argv[optind++]);
        }
        treatedCount += (1 - res);
      }
      printf ("\n%s %d/%d files\n", compress ? "Compressed" : "Decompressed", treatedCount, totalCount);
      putchar ('\n');
    }else{
        printf("Put at least one file to %s\n", compress ? "compress" : "decompress");
        return 1;
    }

  return 0;
}