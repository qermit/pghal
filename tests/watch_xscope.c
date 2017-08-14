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

#include "pghal_spi.h"
#include "wishbone/wb_spi.h"

#include "pghal_i2c.h"
#include "wishbone/wb_i2c.h"

#include "wishbone/wb_gpio_raw.h"
#include "wishbone/xwb_scope.h"

// CHIPS include
#include "chip/chip_isla216p.h"
#include "static_offsets.h"

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);



int main( int argc, char** argv){
  int pattern = 0;
  
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);

  struct pghal_bus * bus = NULL;
  bus = xdma_open_bus("/dev/xdma/card0/user");
  struct xwb_scope * scope1 =  xwb_scope_create_direct(bus, OFFSET_AMC_XWB_SCOPE1);
  struct xwb_scope * scope2 =  xwb_scope_create_direct(bus, OFFSET_AMC_XWB_SCOPE2);
 
  xwb_scope_single_shot(scope1);
  xwb_scope_single_shot(scope2); 
  while(usleep(10000) == 0){
    xwb_scope_registers_download(scope1);
    xwb_scope_registers_download(scope2);
    printf("1: %08X, %08X | 2: %08X, %08X\n", scope1->reg.csr0, scope1->reg.ptr, scope2->reg.csr0, scope2->reg.ptr);  
  }

  
  return 0;
}
