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

#include "static_offsets.h"

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);



int main( int argc, char** argv){
  
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);

  uint32_t rw_data    = 0;
  uint32_t rw_address = 0;
  char *   rw_component_id = NULL;
  uint32_t rw_offset = 0;

  enum { MODE_WRITE, MODE_READ, MODE_DUMPSDB } mode = MODE_READ;

  int opt;

  char * save_ptr;
  char *token = NULL;

  while((opt = getopt(argc, argv, "u:a:w:d")) != -1 ) {
    switch (opt) {
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
    case 'd': mode = MODE_DUMPSDB; break;
    default:
        fprintf(stderr, "Usage: %s [-a address] [-w data] [-d device]\n", argv[0]);
        return 0;
    }
  }

  struct pghal_bus * bus = NULL;
  bus = uart_open_bus("/dev/ttyUSB0");


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
    wb_sdb_rom_dump(sdb_rom); 
  }


  if (rw_component_id != NULL) { free(rw_component_id); }



 return 0;
}
