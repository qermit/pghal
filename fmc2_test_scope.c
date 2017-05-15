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


#include "wishbone/xwb_scope.h"
#include "static_offsets.h"

#define MAP_SIZE 4096UL

struct __attribute__((packed))  xwb_scope_data {
  uint16_t ch0;
  uint16_t ch1;
  uint16_t ch2;
  uint16_t ch3;
  uint64_t reserved;
};

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);



int main( int argc, char** argv){
  int pattern = 0;
  
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);

  struct xdma_node * xdma = NULL;
  xdma = xdma_open_bus("/dev/xdma/card0/user");

  struct xwb_scope * osci = xwb_scope_create_direct(&xdma->bus, OFFSET_AMC_XWB_SCOPE1);
  

  xwb_scope_set_address_range(osci, 0x4000, 0x1000);
  printf("Start: %08X, end: %08X\n", osci->reg.start_addr, osci->reg.end_addr);
  return 0;
  xwb_scope_single_shot(osci);
  while ( xwb_scope_get_csr(osci) & (XWB_SCOPE_CSR0_STATE_ARMED | XWB_SCOPE_CSR0_STATE_TRIGGERED)) {
    usleep(1);
    printf("Wait .. \n");
  }
  xwb_scope_registers_download(osci);  
  
//  int fd = open("/dev/xdma/card0/bypass", O_RDWR | O_SYNC);
//  struct xwb_scope_data * buff;
//  buff = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//  int i;
//  for ( i=0; i< MAP_SIZE / 16; i++) {
//   //printf("data[%3$d]: %1$04X, %2$04X\n", buff[i].ch0, buff[i].ch2, i);
//   printf("data[%3$d]: %1$u, %2$u\n", buff[i].ch0, buff[i].ch2, i);
//  }
//  printf("%d  \n", sizeof(buff));
  return 0;
}
