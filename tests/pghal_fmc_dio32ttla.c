/******************************************************************************
 * Title      : OHWR FMC DIO32 TTL A
 * Project    : Pretty Good Hardware Abstraction Layer
 ******************************************************************************
 * File       : fmc2_config.c
 * Author     : Piotr Miedzik
 * Company    : GSI
 * Created    : 2018-03-28
 * Last update: 2018-06-28
 * Platform   : FPGA-generics
 * Standard   : C
 ******************************************************************************
 * Description:
 * 
 ******************************************************************************
 * Copyleft (ↄ) 2018 Piotr Miedzik
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

#include "wishbone/sdb.h"
#include "wishbone/wb_sdb_rom.h"


// CHIPS include
#include "fmc/fmc_dio32ttla.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "static_offsets.h"

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);

#define HELP_STR "Usage %s:\n" \
                 "  -d --device=DEVICE     device node /dev/...\n" \
                 "  -u --url=ID            format component_id+offset\n" \
                 "  -a --address=ADDRESS   address\n" \
                 "\n" \
                 "  -w --write=DATA        data to write\n" \
                 "  -o --direction=DATA    direction\n" \
                 "  -r --read              read state\n" \
                 "     --set-before-clear  \n" \
                 "\n" \
                 "  -l --set-user-led=BOOL set or clear user LED\n" \
                 "  -c --get-cable-state   get state of cable\n" \
                 "     --get-alert-state   get alert state\n" \
                 "     --interactive       go into interactive mode\n"


static int get_port_state = 0;
static int set_before_clear = 0;
static int get_cable_state = 0;
static int get_alert_state = 0;
static int do_print_help = 0;
static int do_interactive = 0;


static struct option long_options[] = {
    {"device",           required_argument, NULL,             'd' },
    {"url",              required_argument, NULL,             'u' }, // -u format component_id+offset
    {"address",          required_argument, NULL,             'a' }, // -a address

    {"write",            required_argument, NULL,             'w' }, // -w data to write
    {"direction",        required_argument, NULL,             'o' }, // -w direction controll
    {"read",             no_argument,       &get_port_state,   1 }, // -w read input state
    {"set-before-clear", no_argument,       &set_before_clear, 1 }, // -w wrute data

    {"set-user-led",     required_argument, 0,                'l' }, // -w wrute data
    {"get-cable-state",  no_argument,       &get_cable_state,  1 }, // -w wrute data
    {"get-alert-state",  no_argument,       &get_alert_state,  1 }, // -w wrute data

    {"interactive",      no_argument,       &do_interactive, 1 },
    {"help",             no_argument,       &do_print_help,  1 }, //  wrute data
    {0,         0,                 0,  0 }
};
                           
static char short_options[] = "d:u:a:w:o:rl:h";

void print_help(char * argv0)
{
  fprintf(stderr, HELP_STR, argv0); 
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
        c = getopt_long(argc, argv, short_options, long_options, &option_index);
        if (c == -1) break; // end of options
        switch (c) {
          case '?':
            printf("unknown option: \n");
            do_print_help = 1;
            break;
          case ':':
            printf("missing parameter: %d\n", option_index);
            do_print_help = 1;
            break;
          case 0:
            if (long_options[option_index].flag != 0)
             break;
            break;
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

          case 'l': // set user led 
            //if (optarg != NULL)   = strtol(optarg, NULL, 0);
            break;
          default:
            printf("default: %X", c);
            do_print_help = 2; 
        }
   }

  if (do_print_help) {
     print_help(argv[0]);
     exit(EXIT_FAILURE);
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
  wb_sdb_rom_dump(sdb_rom, 0);

 
  if (rw_component_id == NULL) {
    printf("searching for a first FMC DIO32\n");
    exit(EXIT_FAILURE);
  }

  if (rw_component_id != NULL) {
    char component_name[20];
    wb_sdb_get_name_by_ids(sdb_rom, rw_component_id, component_name);
    wb_sdb_get_addr_by_ids(sdb_rom, rw_component_id, &rw_address);
    enum sdb_record_type record_type = wb_sdb_get_type_by_ids(sdb_rom, rw_component_id);

    printf("Component %s: %s: @%08X+%08X\n", rw_component_id, component_name, rw_address, rw_offset);
    printf("component type: 0x%02X\n", record_type);

    struct wb_sdb_rom * sdb_rom2 = wb_sdb_get_sdb_rom_by_ids(sdb_rom, rw_component_id);
    printf("%X - %X\n", sdb_rom, sdb_rom2);

//    if (record_type == sdb_type_bridge) {
//      rw_component_id = 
//    } 
  }
  exit(EXIT_FAILURE); 
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
