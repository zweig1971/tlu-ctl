#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define TRUE 0
#define FALSE -1

#define required_argument 1 
#define optional_argument 2

int main(int argc, char** argv) {
   int opt_index = 0;
   int opt;
   int value;


   static struct option long_options[4] = {
       {"tsp", required_argument, required_argument, 0},
       {"ta",  required_argument, 0},
       {"tb",  required_argument, 0},
       {0,0,0,0}
   };


   opt=getopt_long(argc, argv ,"hvV", long_options, &opt_index);

   switch (opt)
   {
       case 0:
       printf("timestamp %d -- %s\n",opt_index, optarg);
       
       break;
   }

}

