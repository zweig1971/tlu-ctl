#define TLU_H
#define TLU_H

#include <etherbone.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <getopt.h>

namespace GSI_TLU {

using namespace etherbone;
using namespace std;

#define TM_LATCH 	0x100000

#define timestp  	0x000
#define fifo_clr 	0x004
#define trig_amd_stat	0x008
#define trig_amd_set	0x00c
#define trig_amd_clr	0x010
#define trig_edg_stat   0x014
#define trig_edg_setpos	0x018
#define trig_edg_setneg 0x01c
#define ct_lowreg	0x100
#define ct_hireg 	0x104
#define fifo_default	0x400

#define fifo_pop	0x000
#define fifo_fill_cnt	0x004
#define fifo_utc_wd0	0x008
#define fifo_utc_wd1	0x00c
#define fifo_cycl_wd	0x010
#define fifo_step	0x020

#define no_argument 0
#define required_argument 1 
#define optional_argument 2

struct _tlu_data{
  int channel;	
  int reg;
  int wr_value;
  int rd_value;
  bool rdwd;
};


static struct option long_options[] = {
  {"tsp",    no_argument      , 0, 0},
  {"triga",  required_argument, 0, 1},
  {"trige",  required_argument, 0, 2},
  {"fifo",   required_argument, 0, 3},
  {"ct",     no_argument      , 0, 4},
  {"ch",     required_argument, 0, 5},
  {"deg",    no_argument      , 0,'d'}, 
  {"help",   no_argument      , 0,'h'},
  {0,0,0,0}
};







}
