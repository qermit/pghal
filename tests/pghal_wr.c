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


// CHIPS include
#include "chip/chip_ad9510.h"
#include "chip/chip_si57x.h"
#include "chip/chip_isla216p.h"


#include "fmc/fmc_adc250m.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "static_offsets.h"

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);


void print_help(char * argv0)
{
  fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv0); 
}

int main( int argc, char** argv){
  
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);

  struct pghal_bus * bus = NULL;
  char * devicename = NULL;

  uint32_t rw_data    = 0;
  uint32_t rw_address = 0;
  char *   rw_component_id = NULL;
  uint32_t rw_offset = 0;

  enum { MODE_WRITE, MODE_READ, MODE_DUMPSDB } mode = MODE_READ;

  int opt;

  char * save_ptr;
  char *token = NULL;

  int c;
  while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"device",  required_argument, 0,  0 },
            {"url",     required_argument, 0,  0 }, // -u format component_id+offset
            {"address", required_argument, 0,  0 }, // -a address
            {"write",   required_argument, 0,  0 }, // -w wrute data
            {"tree",    no_argument,       0,  0 }, // -t dump sdb tree
            {0,         0,                 0,  0 }
        };
        c = getopt_long(argc, argv, "d:u:a:w:t", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
          case 'd':
            if (devicename != NULL) free(devicename);
            devicename = strdup(optarg);
            break;  
          case 'u':
            token = strtok_r(optarg, "+", &save_ptr);
            rw_component_id = strdup(token);
            token = strtok_r(NULL, "+", &save_ptr);
            if (token != NULL) rw_offset = strtol(token, NULL, 0);
            break;
          case 'a': 
            if (optarg != NULL)  rw_address = strtol(optarg, NULL, 0);
            break;
          case 'w': 
            if (optarg != NULL)  rw_data = strtol(optarg, NULL, 0);
            mode = MODE_WRITE;
            break;
          case 't': mode = MODE_DUMPSDB; break;
          default: 
            print_help(argv[0]);
            exit(EXIT_FAILURE);
        }
   }


  if(devicename == NULL) {
    devicename = strdup("/dev/ttyUSB0");
  }

  if (strncmp("/dev/ttyUSB", devicename, strlen("/dev/ttyUSB")) == 0) {
    bus = uart_open_bus(devicename);
  } else if (strncmp("/dev/xdma/", devicename, strlen("/dev/xdma/")) == 0) {
    bus = xdma_open_bus(devicename);
  } else {
    fprintf(stderr, "Unknown driver for device \"%s\"\n", devicename);
            exit(EXIT_FAILURE);
  }

  struct wb_sdb_rom * sdb_rom = wb_sdb_rom_create_direct(bus, 0x0, 0x0); 
  if (rw_component_id != NULL) {
    wb_sdb_get_addr_by_ids(sdb_rom, rw_component_id, &rw_address);
    printf("Component %s: @%08X+%08X\n", rw_component_id, rw_address, rw_offset);
  }
 
  struct sdb_node_address * node_address = sdb_address_create(bus, rw_address+rw_offset);
  uint32_t rd_data;
  
  if (mode == MODE_READ) {
    pghal_bus_read(bus, &node_address->address, 1, &rd_data);
    printf("DATA @%08X: %08X\n", rw_address+rw_offset, rd_data);
  } else if (mode == MODE_WRITE) {
    pghal_bus_write(bus, &node_address->address, 1, &rw_data);
  } else if (mode == MODE_DUMPSDB) {
    wb_sdb_rom_dump(sdb_rom, 1); 
  }


  if (rw_component_id != NULL) { free(rw_component_id); }



 return 0;
}
