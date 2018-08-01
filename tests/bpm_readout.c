/******************************************************************************
 * Title      : SI57x example configuration
 * Project    : Pretty Good Hardware Abstraction Layer
 ******************************************************************************
 * File       : fmc2_config.c
 * Author     : Piotr Miedzik
 * Company    : GSI
 * Created    : 2017-03-01
 * Last update: 2017-03-02
 * Platform   : FPGA-generics
 * Standard   : C
 ******************************************************************************
 * Description:
 * 
 ******************************************************************************
 * Copyleft (ↄ) 2017 Piotr Miedzik
 *****************************************************************************/

#include "pghal_inc.h"

#include "sdb_bus.h"
#include "wishbone/xwb_scope.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>


void readout_positions(char * filename, int limit) {
   FILE *infile;
   struct bpm_position position;
   infile = fopen(filename, "r");
   if(infile == NULL) return;
   int i = 0;
   printf(" id | count |    pos X   | RSS\n");
   while(fread(&position, sizeof(struct bpm_position), 1, infile)) {
      printf("%3d | %5d | %10.6lf | %10d\n", i++, 
              BPM_POSITION_GET_COUNTER(position),
              BPM_TO_MM(position.posX, BPM_DEFAULT_RADIUS),
              BPM_POSITION_GET_INTENSITY_X(position) );
      if (i > limit && limit > 0) break;
   }
   fclose(infile);
}

int main( int argc, char** argv){
  
  char * filename = NULL;
  int limit = 0;
  int c;
  while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"file",  required_argument, 0,  0 },
            {"count",  required_argument, 0,  0 },
            {0,         0,                 0,  0 }
        };
        c = getopt_long(argc, argv, "f:c:", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
          case 'c':
            limit = strtol(optarg, (char **)NULL, 10);
            break;
          case 'f':
            if (filename != NULL) free(filename);
            filename = strdup(optarg);
            break;
          default: 
            //print_help(argv[0]);
            exit(EXIT_FAILURE);
        }
   }


  if(filename == NULL) {
    return -1;
  }
  readout_positions(filename, limit);
  return 0;
  
}
