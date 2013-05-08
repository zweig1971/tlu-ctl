#include "tlu-ctl.h"

using namespace GSI_TLU;

static const char* program;

static void die(eb_status_t status, const char* what) {
  fprintf(stderr, "%s: %s -- %s\n", program, what, eb_status(status));
  exit(1);
}

void print_tlu_data(int tlu_index, struct _tlu_data tlu_data[]){

  int index;
  printf("\nnumber of :%d\n", tlu_index);
  printf("------------------\n");

  for (index=1; index <= tlu_index; index++){
    printf("[%d].channel :%x\n", index, tlu_data[index].channel);    	
    printf("[%d].register:%x\n", index, tlu_data[index].reg);
    printf("[%d].wr_value:%x\n", index, tlu_data[index].wr_value);
    printf("[%d].rd_value:%x\n", index, tlu_data[index].rd_value);
    printf("[%d].rdwd    :%d\n", index, tlu_data[index].rdwd);
    printf("------------\n");
  }  
}

static void help(void){

  fprintf(stderr, "\n");
  fprintf(stderr, "  --tsp                read the timestamp\n");
  fprintf(stderr, "  --triga <command>    status/set/clear the armed trigger\n");
  fprintf(stderr, "  --trige <command>    status/set the trigger\n");
  fprintf(stderr, "  --fifo  <command>    do some cool things with the fifo\n");
  fprintf(stderr, "  --ch    <channel id> set the channel in hex, no channel -> all channels\n");
  fprintf(stderr, "  --ct                 read the clicks from beginning of 1971");
  fprintf(stderr, "\n");
  fprintf(stderr, "commands for the trigger\n");
  fprintf(stderr, "  status                            read the status from the armed/edge trigger\n");
  fprintf(stderr, "  arm                               set the trigger\n");
  fprintf(stderr, "  clear                             clear the armed trigger\n");
  fprintf(stderr, "  neg                               set neg edge \n");
  fprintf(stderr, "  pos                               set pos edge \n");
  fprintf(stderr, "\n");
  fprintf(stderr, "commands for the fifo\n");
  fprintf(stderr, "  pop                               clear the fifo for this channel\n");
  fprintf(stderr, "  count                             count the elements\n");
  fprintf(stderr, "  cycle                             cycle counter\n");
  fprintf(stderr, "  utc                               read the clicks from beginning of 1971\n");
  fprintf(stderr, "\n");
}


int main(int argc, char** argv) {
  int opt,error,index,opt_index, tlu_index,running;
  const char *devpath, *command;
  bool debug = false;
  char *value_end;
  eb_status_t status;
  string test;
  
  eb_address_t  address;
  eb_format_t   format = EB_ADDR32|EB_DATA32; 
  eb_data_t	data;

  struct _tlu_data tlu_data[32];

  program = argv[0];
  error = 0;
  tlu_index=0;
  while ((opt = getopt_long(argc, argv, "h",long_options, &opt_index )) != -1){
    switch(opt){
      case 0:
	// ----- timestamp -----
	tlu_index = tlu_index+1;
	tlu_data[tlu_index].reg = timestp;
	tlu_data[tlu_index].channel  = 0;
	tlu_data[tlu_index].wr_value = 0;
        tlu_data[tlu_index].rd_value = 0;
	tlu_data[tlu_index].rdwd     = true;
	break;
      case 1:
	 // ----- trigger armed -----
	tlu_index = tlu_index+1;
	tlu_data[tlu_index].channel  = 0;
        tlu_data[tlu_index].wr_value = 0;
        tlu_data[tlu_index].rd_value = 0;
	if(string(optarg) == "status") {
	  tlu_data[tlu_index].reg = trig_amd_stat;
	  tlu_data[tlu_index].rdwd= true;
	} else if (string(optarg) == "arm"){
	  tlu_data[tlu_index].reg = trig_amd_set;
	  tlu_data[tlu_index].rdwd= false;
	} else if (string(optarg) == "clr"){
	  tlu_data[tlu_index].reg = trig_amd_clr;
          tlu_data[tlu_index].rdwd= false;
	} else {printf("-- Command not found:%s\n",optarg);return -1;}  
        
	break;
      case 2:
         // ----- trigger edge  -----
        tlu_index = tlu_index+1;
	tlu_data[tlu_index].channel  = 0;
        tlu_data[tlu_index].wr_value = 0;
        tlu_data[tlu_index].rd_value = 0;
        if(string(optarg) == "status") {
          tlu_data[tlu_index].reg = trig_edg_stat;
          tlu_data[tlu_index].rdwd= true;
        } else if (string(optarg) == "pos"){
          tlu_data[tlu_index].reg = trig_edg_setpos;
          tlu_data[tlu_index].rdwd= false;
        } else if (string(optarg) == "neg"){
          tlu_data[tlu_index].reg = trig_edg_setneg;
          tlu_data[tlu_index].rdwd= false;
        }
        break;
      case 3:
	//  ----- fifo  ----        
        tlu_index = tlu_index+1;
        tlu_data[tlu_index].channel  = 0;
        tlu_data[tlu_index].wr_value = 0;
        tlu_data[tlu_index].rd_value = 0;
        if(string(optarg) == "pop") {
          tlu_data[tlu_index].reg = fifo_default+trig_edg_stat;
          tlu_data[tlu_index].rdwd= false;
        } else if (string(optarg) == "count") {		
          tlu_data[tlu_index].reg = fifo_default+fifo_fill_cnt;
          tlu_data[tlu_index].rdwd= true;
        } else if (string(optarg) == "utc") {
	  // read the utc word 0
          tlu_data[tlu_index].reg = fifo_default+fifo_utc_wd0;
          tlu_data[tlu_index].rdwd= true;
	  // read the utc word 1
          tlu_index = tlu_index+1;
          tlu_data[tlu_index].channel  = 0;
          tlu_data[tlu_index].wr_value = 0;
          tlu_data[tlu_index].rd_value = 0;
          tlu_data[tlu_index].reg = fifo_default+fifo_utc_wd1;
          tlu_data[tlu_index].rdwd= true;
	} else if (string(optarg) == "cycle") {
          tlu_data[tlu_index].reg = fifo_default+fifo_cycl_wd;
          tlu_data[tlu_index].rdwd= false;
	}
        break;
      case 4:
        // ----- ct ---- 
        // read the high byte
        tlu_index = tlu_index+1;
	tlu_data[tlu_index].channel  = 0;
	tlu_data[tlu_index].reg      = ct_hireg; 
        tlu_data[tlu_index].wr_value = 0;
        tlu_data[tlu_index].rd_value = 0;
	tlu_data[tlu_index].rdwd     = true;
        // read the low byte
        tlu_index = tlu_index+1;
	tlu_data[tlu_index].channel  = 0;
        tlu_data[tlu_index].reg      = ct_lowreg;
        tlu_data[tlu_index].wr_value = 0;
        tlu_data[tlu_index].rd_value = 0;
        tlu_data[tlu_index].rdwd     = true; 
        break;
      case 5:
        //  ----- read channel  -----
        tlu_data[tlu_index].channel = strtoul(optarg, 0, 16);
        break;
      case 'd':
        // --- debug ---
        debug = true; 
        break;
      case 'h':
        help();
        return 0;
    }//case
  }//while

  if (optind >= argc) {
     fprintf(stderr, "%s: expecting one non-optional argument: <etherbone-device>\n", program);
     fprintf(stderr, "\n");
     help();
     return 1;
  }

  // device path
  devpath = argv[optind];

  // for debug
  if (debug){
     printf("---------------\n");
     printf("devpath :%s\n",devpath);
     printf("\ncollected data :\n");
     print_tlu_data(tlu_index, tlu_data);  
  }

  
  if(debug){printf("- now open socket...\n");};
  Socket socket;
  if ((status = socket.open()) != EB_OK) die(status, "etherbone::socket.open");

  if(debug){printf("- now open device...\n");};
  Device device;
  if ((status = device.open(socket, devpath)) != EB_OK) {
    fprintf(stderr, "%s: etherbone::device.open('%s') -- %s\n", program, devpath, eb_status(status));
    socket.close();
    return 1;
  }


// ----------- Cycle fuellen -----------


  if(debug){printf("- now open cycle...\n");};
  Cycle cycle;
  if ((status= cycle.open(device))!=EB_OK){
    die(status, "cycle.open");
  };


  cycle.read(0x100000, format, &data);

/*

  index=0;
  while (index < tlu_index){
    index++;
    // lese auftrag
    if (tlu_data[tlu_index].rdwd){
      // adresse bauen
      address = TM_LATCH + tlu_data[tlu_index].reg; 
      // lese cycle fuellen
      //cycle.read(address, format, &data);
      cycle.read(0x100000, format, &data);
      //debug
      if(debug){printf("- read [%d]cycle data : -address: %lX -data: %lX\n", index, address, data);};
      // daten uebernehmen
      tlu_data[index].rd_value = data;
    } else { // schreibe auftrag
       // adresse bauen
      address = TM_LATCH + tlu_data[tlu_index].reg;
      // daten uebernehmen
      data =  tlu_data[index].wr_value;
      //debug
      if(debug){printf("- write [%d]cycle data : -adresse: %lX -data: %lX\n", index, address, data);};
      // schreibe cycle fuellen
      cycle.write(address, format, data);
     }
  }//while  

*/

  printf("data :%x  ---  %d\n", data,data);




  // auftrag beendet

  // cycle schliessen
  if(debug){printf("- now close cycle...\n");};
  cycle.close();

  //device schliessen
  if(debug){printf("- now close device...\n");};
  device.close();

  //socket schliessen
  if(debug){printf("- now open socket...\n");};
  socket.close();
}







