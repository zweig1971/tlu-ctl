#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define TRUE 0
#define FALSE -1

int main(int argc, char** argv) {

   int opt;

   printf("Insgesamt %d Argumente\n",argc-1);
   printf("Letztes Argument: %s\n",argv[argc-1]);


   opt = getopt(argc, argv, "a:e:c:s:vqnh");
   printf("getopt :%d\n",optind);
   printf("optarg :%s\n",optarg);


}
