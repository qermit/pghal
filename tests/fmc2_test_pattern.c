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

// CHIPS include
#include "chip/chip_isla216p.h"
#include "static_offsets.h"

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);



int main( int argc, char** argv){
  int pattern = 0;
  
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);

  struct pghal_bus * bus = xdma_open_bus("/dev/xdma/card0/user");


  struct wb_spi * spi_isla216p = wb_spi_init(bus, OFFSET_FMC2 | OFFSET_ADC250_SPI_ISLA);

  int isla_chips = 4;
  struct chip_isla216p * fmc2_isla216p[4];
  // reset first, wait for all in separate loop
  { int i;
    for (i = 0 ; i<isla_chips; i++){
      fmc2_isla216p[i] = chip_isla216p_init(&spi_isla216p->spi);
      chip_isla216p_set_address(fmc2_isla216p[i], i);
      chip_isla216p_registers_download(fmc2_isla216p[i], CHIP_ISLA216P_CAL_ID);
     
    
      if (pattern == 1) {
      chip_isla216p_test_path(fmc2_isla216p[i], ISLA216P_TEST_PATTERN_USER1, 0xFFFF, 0x0000, 0x0000, 0x0000);
      } else if (pattern == 2) {
      chip_isla216p_test_path(fmc2_isla216p[i], ISLA216P_TEST_PATTERN_USER2, 0xAA55, 0x55AA, 0x0000, 0x0000);
      } else if (pattern == 3) {
      fmc2_isla216p[i]->reg_current.test[0]=0x80;

      fmc2_isla216p[i]->reg_current.test[2]=0x00;
      fmc2_isla216p[i]->reg_current.test[1]=0x99;
      fmc2_isla216p[i]->reg_current.test[3]=0xFF;
      fmc2_isla216p[i]->reg_current.test[4]=0xFF;
      } else if (pattern == 4) {
        chip_isla216p_test_path(fmc2_isla216p[i], ISLA216P_TEST_PATTERN_RAMP, 0x0000, 0x0000, 0x0000, 0x0000);
      } else {
        //chip_isla216p_test_path(fmc2_isla216p[i], ISLA216P_TEST_PATTERN_RAMP, 0x0000, 0x0000, 0x0000, 0x0000);
        chip_isla216p_test_path(fmc2_isla216p[i], ISLA216P_TEST_PATTERN_OFF, 0x0000, 0x0000, 0x0000, 0x0000);
      }
      //chip_isla216p_registers_upload(fmc2_isla216p[i], CHIP_ISLA216P_TEST_ID, NULL);
    }
    
  }
  
  

  
  return 0;
}
