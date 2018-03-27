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
#include "wishbone/wb_sdb_rom.h"

#include "wishbone/wb_gpio_raw.h"
#include "wishbone/xwb_scope.h"

// CHIPS include
#include "chip/chip_isla216p.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "static_offsets.h"

#include <sys/time.h>

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);

void print_help(char * argv0)
{
  fprintf(stderr, "Usage: %s -d device_name\n", argv0); 
}

struct watch_struct {
  struct pghal_bus * bus;
  char * devicename;
  struct wb_sdb_rom * sdb_rom;
  struct xwb_scope * scope1;
  struct xwb_scope * scope2;
};

int main( int argc, char** argv){
  int pattern = 0;
  
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);

  int count_devices = 0;
  struct watch_struct * devices[5] = { NULL, NULL, NULL, NULL, NULL };

  uint32_t rw_data    = 0;
  uint32_t rw_address = 0;
  char *   rw_component_id = NULL;
  uint32_t rw_offset = 0;

  enum { MODE_WRITE, MODE_READ, MODE_DUMPSDB } mode = MODE_READ;

  int opt;

  char * save_ptr;
  char *token = NULL;

  int c;
  int i;
  while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"device",  required_argument, 0,  0 },
            {"help",  no_argument, 0,  0 },
            {0,         0,                 0,  0 }
        };
        c = getopt_long(argc, argv, "d:h", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
          case 'd':
            for(i = 0; i<5 && devices[i]!=NULL;  i++){
              if (strcmp(optarg, devices[i]->devicename) != 0) {
                continue;
              }
              printf("device %s already exists\n", optarg); break;
            }
            printf("I'm here, i:%d\n", i);
            if (i >= 5) { printf("to many devices\n"); break; }
            if (devices[i] != NULL) { printf("device already exists, skipping\n"); break; }
            devices[i] = (struct watch_struct *) malloc(sizeof(struct watch_struct));
            devices[i]->devicename = strdup(optarg);
            devices[i]->bus = NULL;
            devices[i]->scope1 = NULL;
            devices[i]->scope2 = NULL;
            count_devices++;
            printf("I'm still here\n");
            break;
          case 'h':
            print_help(argv[0]);
            exit(0);
          default: 
            print_help(argv[0]);
            exit(EXIT_FAILURE);
        }
   }

 if (devices[0] == NULL ) {
   fprintf(stderr, "You need to define at least one device\n");
   return 0;
 }
//  if(devicename == NULL) {
//    devicename = strdup("/dev/xdma/card0/user");
//  }

  for(i = 0; i<count_devices; i++){
    if (strncmp("/dev/ttyUSB", devices[i]->devicename, strlen("/dev/ttyUSB")) == 0) {
      devices[i]->bus = uart_open_bus(devices[i]->devicename);
    } else if (strncmp("/dev/xdma/", devices[i]->devicename, strlen("/dev/xdma/")) == 0) {
      devices[i]->bus = xdma_open_bus(devices[i]->devicename);
    } else {
      fprintf(stderr, "Unknown driver for device \"%s\"\n", devices[i]->devicename);
              exit(EXIT_FAILURE);
    }
    devices[i]->sdb_rom = wb_sdb_rom_create_direct(devices[i]->bus, 0x0, 0x0);
    devices[i]->scope1 =  xwb_scope_create_direct(devices[i]->bus, OFFSET_AMC_XWB_SCOPE1);
    devices[i]->scope2 =  xwb_scope_create_direct(devices[i]->bus, OFFSET_AMC_XWB_SCOPE2);
    //xwb_scope_registers_download(devices[i]->scope1);
    //xwb_scope_registers_download(devices[i]->scope2);
  }

  struct timeval tv;
  while(usleep(100000) == 0){
    struct xwb_scope * scope1;
    struct xwb_scope * scope2;
    gettimeofday(&tv, NULL);
    printf("%d - ", tv.tv_sec);
    for(i = 0; i< count_devices; i++){
      scope1 = devices[i]->scope1;
      scope2 = devices[i]->scope2;
      xwb_scope_registers_download(scope1);
      xwb_scope_registers_download(scope2);
      printf("%d.1: %08X, %08X | %d.2: %08X, %08X | ", i, scope1->reg.csr0, scope1->reg.ptr, i, scope2->reg.csr0, scope2->reg.ptr);  
    }
    printf("\n");
  }
/*

  struct xwb_scope * scope1 =  xwb_scope_create_direct(bus, OFFSET_AMC_XWB_SCOPE1);
  struct xwb_scope * scope2 =  xwb_scope_create_direct(bus, OFFSET_AMC_XWB_SCOPE2);
 
//  xwb_scope_single_shot(scope1);
//  xwb_scope_single_shot(scope2); 
  while(usleep(10000000) == 0){
    xwb_scope_registers_download(scope1);
    xwb_scope_registers_download(scope2);
    printf("%d 1: %08X, %08X | 2: %08X, %08X\n", tv.tv_sec, scope1->reg.csr0, scope1->reg.ptr, scope2->reg.csr0, scope2->reg.ptr);  
  }
*/
  
  return 0;
}
