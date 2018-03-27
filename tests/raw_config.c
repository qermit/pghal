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
#include "sdb_uart.h"
#include "sdb_xdma.h"
#include "sdb_xdma_priv.h"


#include "pghal_spi.h"
#include "wishbone/wb_spi.h"

#include "pghal_i2c.h"
#include "wishbone/wb_i2c.h"

#include "wishbone/wb_gpio_raw.h"

#include "wishbone/wb_sdb_rom.h"
#include "wishbone/xwb_scope.h"


// CHIPS include
#include "chip/chip_ad9510.h"
#include "chip/chip_si57x.h"
#include "chip/chip_isla216p.h"


#include "fmc/fmc_adc250m.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

//#include "static_offsets.h"

#define DEVICE_NAME_DEFAULT "/dev/xdma/card0/h2c0"
#define DATA_FILENAME_DEFAULT "/home/pmiedzik/data/20171123_103115.188_4408c8522c3004_1.dat"
#define DATA_SIGNED 1

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);


struct data_raw_in {
  uint16_t ver1;
  uint16_t hor1;
  uint16_t ver2;
  uint16_t hor2;
};

struct data_adc_in {
  int16_t hor1;
  int16_t hor2;
  int16_t ver1;
  int16_t ver2;
  int16_t hor3;
  int16_t hor4;
  int16_t ver3;
  int16_t ver4;
};

struct watch_struct {
  struct pghal_bus * bus;
  char * devicename;
  struct wb_sdb_rom * sdb_rom;
  int scope_count;
  struct xwb_scope ** scope;
};


void print_summary(struct chip_si57x * chip, struct chip_si57x_regs * regs) 
{
  printf("===============================\n");
  printf("Chip si57x summary:\nfout: %lf\nfxtal: %lf MHz\n", regs->fout, chip->fxtal);
  printf("HS_DIV: %d\nN1: %d\n", regs->hsdiv, regs->n1);
  printf("RFREQ: %lf\n", regs->rfreq);
  printf("FDCO: %lf\n", regs->fout * (regs->hsdiv * regs->n1));
  pghal_dump_regs(regs->regs_raw, 6);
  printf("===============================\n");
}

void print_help(char * argv0)
{
  fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv0); 
}

void push_data(char *devicename, char *datafile) {
  uint32_t size = 4096;
  char *allocated = NULL;

  int c_sampling_speed  =  125000000; // 125MHz;
  int c_sampling_period = 1000000000 / c_sampling_speed; // c_sampling_speed
  //int c_data_skip = 200000; // skip data 
  int c_data_skip = 0; // skip data 
  printf("Sampling period = %d\n", c_sampling_period);

  //int trigger_rise_timestamp[] = { 20000,  95000 };
  //int trigger_fall_timestamp[] = { 30000, 100000 };
  int trigger_rise_timestamp[] = { 20000 };
  int trigger_fall_timestamp[] = { 95000 };
  int trigger_count = sizeof(trigger_rise_timestamp)/sizeof(int);
  int trigger_id = 0;

  int rf_period = 7100;
  int rf_duty   = 3550;
  int rf_delay  = 100;

  int rf_state  = 0; 
  int trigger_state = 0;

  int next_event;
  int next_event_trig;
  int next_event_rf;

  int current_timestamp = 0;
  int current_sample    = 0;
  int file_offset       = 0;

  posix_memalign((void **)&allocated, 4096/*alignment*/, size + 4096);
  assert(allocated);
  //buffer = allocated + offset;

   
  int fpga_fd = open(devicename, O_RDWR);
  assert(fpga_fd >= 0);

  int file_fd = open(datafile, O_RDONLY);
  assert(file_fd >= 0);  

  off_t data_skip_off = sizeof(struct data_raw_in) * c_data_skip;
  lseek(file_fd, data_skip_off, SEEK_SET);
  struct data_raw_in raw_in;
  
  while(1) {
    current_timestamp = current_sample * c_sampling_period;
    printf("current sample: %d, timestamp: %d ns\n", current_sample, current_timestamp);
    int rf_point = ((current_timestamp - rf_delay) % rf_period);
    if (rf_point < rf_duty) {
      rf_state = 0;
      next_event_rf = rf_duty - rf_point;
    } else {
      rf_state = 1;
      next_event_rf = rf_period - rf_point;
    } 

    

    if (trigger_id < trigger_count) {
      trigger_state = 0;
      if (current_timestamp >= trigger_rise_timestamp[trigger_id]) {
        trigger_state = 1;
      }
      if (current_timestamp >= trigger_fall_timestamp[trigger_id]) {
        trigger_state = 0;
        trigger_id ++;
      }


      if (current_timestamp < trigger_rise_timestamp[trigger_id]) {
        next_event_trig = trigger_rise_timestamp[trigger_id] - current_timestamp;
      } else {
        next_event_trig = trigger_fall_timestamp[trigger_id] - current_timestamp;
      }
    }
    printf("next_event_rf %d\n", next_event_rf);
    next_event = 4096 / 16; //== 256
    if ((trigger_id < trigger_count) && (next_event_rf > next_event_trig)) {
       next_event = next_event_trig / c_sampling_period;
       if (next_event_trig % c_sampling_period) next_event++;
    } else {
       next_event = next_event_rf / c_sampling_period;
       if (next_event_rf % c_sampling_period) next_event++;
    }
    if (next_event > 256) next_event = 256;
    int i;
    struct data_adc_in * adc_data = (struct data_adc_in *) allocated;
    for(i = 0; i<next_event; i++) {
      read(file_fd, &raw_in, sizeof(raw_in));
      adc_data[i].hor1 = raw_in.hor1;
      adc_data[i].hor2 = raw_in.hor2;
      adc_data[i].ver1 = raw_in.ver1;
      adc_data[i].ver2 = raw_in.ver2;
    }
    printf("feeding length = %d (writing @0x%08X\n", next_event, ((trigger_state << 13) | (rf_state << 12)));
    off_t off = lseek(fpga_fd, ((trigger_state << 13) | (rf_state << 12)) , SEEK_SET);
    write(fpga_fd, allocated, next_event * sizeof(struct data_adc_in)); 
 //
 //
   current_sample += next_event;
   //break;   
   usleep(100000);
  }

  close(file_fd);
  close(fpga_fd);

}


int main( int argc, char** argv){
  
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);
  
  int fmc_id = 1;
  double fnew = 125.0;
  double fnew_tmp = 100.0;

  struct watch_struct cfg_struct = {
    .bus = NULL,
    .devicename = NULL,
    .sdb_rom = NULL,
    .scope_count = 0,
    .scope = NULL 
  };

  int c;
  int digit_optind = 0;
  while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"device",  required_argument, 0,  0 },
            {"fmc_id",  required_argument, 0,  0 },
            {"freq",    required_argument, 0,  0 },
            {0,         0,                 0,  0 }
        };
        c = getopt_long(argc, argv, "d:i:f:", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
          case 'd':
            if (cfg_struct.devicename != NULL) free(cfg_struct.devicename);
            cfg_struct.devicename = strdup(optarg);
            break;  
          case 'i':
            if (optarg != NULL)  fmc_id = strtol(optarg, NULL, 0); 
            break;
          case 'f':
            fnew_tmp = strtod(optarg, NULL);
            // @todo: add some checks
            fnew = fnew_tmp;
            break;
          default: 
            print_help(argv[0]);
            exit(EXIT_FAILURE);
        }
   }

  if (fmc_id < 1 || fmc_id > 2) fmc_id = 1;

  if(cfg_struct.devicename == NULL) {
    cfg_struct.devicename = strdup("/dev/ttyUSB0");
  }

  if (strncmp("/dev/ttyUSB", cfg_struct.devicename, strlen("/dev/ttyUSB")) == 0) {
     cfg_struct.bus = uart_open_bus(cfg_struct.devicename);
  } else if (strncmp("/dev/xdma/", cfg_struct.devicename, strlen("/dev/xdma/")) == 0) {
     cfg_struct.bus = xdma_open_bus(cfg_struct.devicename);
  } else {
    fprintf(stderr, "Unknown driver for device \"%s\"\n",  cfg_struct.devicename);
            exit(EXIT_FAILURE);
  }

  struct wb_sdb_rom * sdb_rom = wb_sdb_rom_create_direct(cfg_struct.bus, 0x0, 0x0); 
  // @todo: automagic fmc search
  //
  uint32_t OFFSET_FMC = 0;
  uint16_t fmc_component_id = 0;
  {
    int i = 10;
    char s_name[32];
    char tmp_name[33];
      
    int max_component_id = 8;
    sprintf(s_name, "XWB SCOPE"); 
    for (i=3; i <= max_component_id; i++){
      int len = wb_sdb_get_name_by_id(sdb_rom, i, tmp_name);
      if (len < 0) continue;
      tmp_name[len] = 0;
      if (strncmp(s_name, tmp_name, strlen(s_name)) != 0) continue;
      if (cfg_struct.scope_count == 0) {
        cfg_struct.scope = malloc(sizeof(struct xwb_scope*)*10);
      }  
      
      wb_sdb_get_addr_by_id(sdb_rom, i, &OFFSET_FMC);
      printf("registring %s@0x%08X\n", tmp_name, OFFSET_FMC);
      //struct xwb_scope * scope = xwb_scope_create_direct(cfg_struct.bus, OFFSET_FMC);
      cfg_struct.scope[cfg_struct.scope_count] = xwb_scope_create_direct(cfg_struct.bus, OFFSET_FMC);
      cfg_struct.scope_count++;
    }

    
    if (cfg_struct.scope_count == 0) {
       printf("component %s not found\n", s_name);
       exit(1);
    }

  }

  xwb_scope_set_address_range(cfg_struct.scope[1], 0, 0x10000000);
  xwb_scope_set_address_range(cfg_struct.scope[0], 0x10000000, 0x100000);
  int i;

  for (i=0 ; i<cfg_struct.scope_count; i++) {
       xwb_scope_registers_download(cfg_struct.scope[i]);
      printf("scope: %d\nCSR0: %08X\nPTR: %08X\nSTART: %08X\nEND: %08X\n", i, cfg_struct.scope[i]->reg.csr0,
        cfg_struct.scope[i]->reg.ptr,
        cfg_struct.scope[i]->reg.start_addr,
        cfg_struct.scope[i]->reg.end_addr); 
  }
 
  xwb_scope_single_shot(cfg_struct.scope[0]);
  //xwb_scope_single_shot(cfg_struct.scope[0]);
  //xwb_scope_until_oom_shot(cfg_struct.scope[0]);
  //xwb_scope_circular_shot(cfg_struct.scope[0]);
 
  push_data(DEVICE_NAME_DEFAULT, DATA_FILENAME_DEFAULT);
  exit(0);  
  
}
