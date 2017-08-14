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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include "sdb_bus.h"
#include "sdb_xdma.h"
#include "sdb_uart.h"


#include "static_offsets.h"

#include "wishbone/wb_xilinx_dna.h"
#include "wishbone/wb_gpio_raw.h"
#include "wishbone/wb_fmc_csr.h"
#include "wishbone/wb_sdb_rom.h"

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);


void init_procedure(struct pghal_bus * bus, char * card_name) {

  struct wb_sdb_rom * sdb_rom = wb_sdb_rom_create_direct(bus, 0x0, 0x0);
  uint32_t tmp_addr;
  int len;
//  int len =  wb_sdb_get_name_by_id(sdb_rom, device_id, tmp_name);
//  wb_sdb_get_addr_by_id(sdb_rom, device_id, &tmp_addr);

  struct wb_xilinx_dna * dna = NULL;
  struct wb_gpio_raw * afc_gpio = NULL;
  struct wb_gpio_raw * fmc1_dio_gpio = NULL;
  struct wb_fmc_csr * fmc1_csr  = NULL;

  len = wb_sdb_get_addr_by_id(sdb_rom, "1", &tmp_addr);
  if (len >= 0)
     dna = wb_xilinx_dna_create_direct(bus, tmp_addr);
  wb_xilinx_dna_registers_download(dna);
  printf("Card: %s\n EFUSE: %08X\n ACCESS2: %08X\n DNA: %016lX\n", card_name, dna->reg.efuseusr, dna->reg.access2, dna->reg.dna);



  wb_sdb_get_addr_by_id(sdb_rom, "2", &tmp_addr);
  afc_gpio = wb_gpio_raw_create_direct(bus, tmp_addr);

  wb_sdb_get_addr_by_id(sdb_rom, "4.4", &tmp_addr);
  fmc1_dio_gpio = wb_gpio_raw_create_direct(bus, tmp_addr);

  wb_sdb_get_addr_by_id(sdb_rom, "4.1", &tmp_addr);
  fmc1_csr = wb_fmc_csr_create_direct(bus, tmp_addr);


  wb_gpio_raw_set_port_direction(afc_gpio, 0x700);
  wb_gpio_raw_set_port_value(afc_gpio, 0x100, 0x700);
  usleep(500000);
  wb_gpio_raw_set_port_value(afc_gpio, 0x200, 0x700);
  usleep(500000);
  wb_gpio_raw_set_port_value(afc_gpio, 0x400, 0x700);
  usleep(500000);
  wb_gpio_raw_set_port_value(afc_gpio, 0x000, 0x700);

  wb_fmc_csr_enable(fmc1_csr, 1);

  wb_gpio_raw_set_port_direction(fmc1_dio_gpio, 0x060);
  wb_gpio_raw_set_port_value(fmc1_dio_gpio, 0x060, 0x060);
  usleep(500000);
  wb_gpio_raw_set_port_value(fmc1_dio_gpio, 0x000, 0x060);


}

int read_card_ttyUSB(char* card_name){
  char card_dev_name[256];
  
  sprintf(card_dev_name, "%s", card_name);

  printf("open card |%s|\n", card_dev_name);
  struct pghal_bus * bus = NULL;
  bus = uart_open_bus(card_dev_name);

  init_procedure(bus, card_name);
  // todo: destroy functions
 
  
  return 0;
}
/*
int read_card(char* card_name){
  char card_dev_name[256];
  
  sprintf(card_dev_name, "/dev/xdma/%s/user", card_name);

  struct xdma_node * xdma = NULL;
  xdma = xdma_open_bus(card_dev_name);

//struct xwb_xilinx_dna * dna;

//  struct wb_xilinx_dna * dna = wb_xilinx_dna_create_direct(&xdma->bus, 0);
  struct wb_xilinx_dna * dna = wb_xilinx_dna_create_direct(&xdma->bus, OFFSET_AMC_DNA);

//  wb_xilinx_dna_create_direct(&xdma->bus, OFFSET_AMC_DNA);


  wb_xilinx_dna_registers_download(dna);

//  printf("Card: %s\n EFUSE: %08X\n", card_name, dna->reg.efuseusr);
  printf("Card: %s\n EFUSE: %08X\n ACCESS2: %08X\n DNA: %016lX\n", card_name, dna->reg.efuseusr, dna->reg.access2, dna->reg.dna);

  // todo: destroy functions
 
  
  return 0;
}
*/



int main (void)
{
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);
  uart_bus_driver_register(&bus_drivers_head);
  uart_node_driver_register(&node_drivers_head);

  read_card_ttyUSB("/dev/ttyUSB0");


  return 0;
/*
  DIR *dp;
  struct dirent *ep;     
  dp = opendir ("/dev/xdma");

  if (dp != NULL)
  {
    while (ep = readdir (dp)) {
      if (strcmp(ep->d_name, "..") && strcmp(ep->d_name, "."))
        read_card(ep->d_name);
    }

    (void) closedir (dp);
  }
  else
    perror ("Couldn't open the directory");
*/
  return 0;
}

