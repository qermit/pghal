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
#include "sdb_xdma.h"
#include "sdb_xdma_priv.h"

#include "static_offsets.h"

#include "wishbone/wb_xilinx_dna.h"

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);


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




int main (void)
{
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);


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

  return 0;
}

