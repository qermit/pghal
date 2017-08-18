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

#include "static_offsets.h"

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);


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

int main( int argc, char** argv){
  
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);

  int fmc_id = 2;
  uint32_t OFFSET_FMC = OFFSET_FMC1;
  if (argc > 1) {
   if (argv[1][0] == '1') {
     fmc_id = 1;
   } else if (argv[1][0] == '2' ) {
     fmc_id = 2;
   }
  }

  if (fmc_id == 1) {
   OFFSET_FMC = OFFSET_FMC1;
  } else if (fmc_id == 2) {
   OFFSET_FMC = OFFSET_FMC2;
  }
  
  struct pghal_bus * bus = NULL;
  // bus = xdma_open_bus("/dev/xdma/card4/user");
  bus = uart_open_bus("/dev/ttyUSB0");
  struct wb_sdb_rom * sdb_rom = wb_sdb_rom_create_direct(bus, 0x0, 0x0); 
  
  struct fmc_adc250m * fmc2 = fmc_adc250m_init(bus, 0x2000, 0x2000);

  // set trigger as input
  wb_gpio_raw_set_port_direction(fmc2->gpio, 0x2000 | 0x20);
  wb_gpio_raw_set_port_value(fmc2->gpio, 0x2000 | 0x20, 0x2000 | 0x20); 

  wb_gpio_raw_set_port_termination(fmc2->gpio, 0x8); // <- trigger input termination enable
  wb_gpio_raw_set_port_altf(fmc2->gpio, 0x0);        // <- reset all alternative functions
  wb_gpio_raw_set_port_value(fmc2->gpio, 0x20, 0x20 | 0x2000); 

  wb_gpio_raw_set_port_value(fmc2->gpio, 0x0400, 0x0400);
//  wb_gpio_raw_set_port_value(fmc2->gpio, 0x0000, 0x0400);
//  usleep(1000);
//  */
  //return 0; 
  // debug spi and i2c over FMC1 dio;
  // it's bitstream specific
//  wb_gpio_raw_set_port_altf(fmc1_gpio, 0b00000110); 
//  wb_gpio_raw_set_port_direction(fmc1_gpio, 0b00000110); 


  double fnew = 100.0;

  if (1) { 
  if (0) {
    chip_si57x_decode_part_number(fmc2->chip_si57x, "TESTTESTTEST01");
    fnew = 161.132812;
  } else {
    chip_si57x_decode_part_number(fmc2->chip_si57x, "571AJC000337DG");
    fnew = 125.0;
  }  
  printf("Fnew: %lf\n", fnew);

  chip_si57x_reload_initial(fmc2->chip_si57x);
  //return 0;
  chip_si57x_find_valid_combo(fmc2->chip_si57x, fnew, SI57X_METHOD_HARD);
  chip_si57x_send_regs(fmc2->chip_si57x, &fmc2->chip_si57x->reg_new, SI57X_METHOD_HARD);
  chip_si57x_registers_download(fmc2->chip_si57x);


//  return 0;
  // enbable VCXO and PLL
    wb_gpio_raw_set_port_value(fmc2->gpio, 0x00, 0x20); 
  //  usleep(1);
    wb_gpio_raw_set_port_value(fmc2->gpio, 0x2000 | 0x20, 0x2000 | 0x20); 
  }
  //return 0;

  if (1) {
    printf("configure AD9510\n");
  // configure AD9510

    chip_ad9510_set_address(fmc2->chip_ad9510, 0);
    chip_ad9510_soft_reset(fmc2->chip_ad9510);


//  return 0;
    chip_ad9510_registers_download(fmc2->chip_ad9510, 0, 100);
    chip_ad9510_config1(fmc2->chip_ad9510);
    chip_ad9510_registers_download(fmc2->chip_ad9510, 0, 100); // donload to assert;
    chip_ad9510_registers_commit(fmc2->chip_ad9510);
  
  }


  //configure 4x ADC
  if (1) {
  // assert and deassert ADC resets
  wb_gpio_raw_set_port_value(fmc2->gpio, 0x0000, 0x0400);
  usleep(10000);
  wb_gpio_raw_set_port_value(fmc2->gpio, 0x0400, 0x0400);
  usleep(10000);
  

  int isla_chips = 4;
  // reset first, wait for all in separate loop
  { int i;
    
    for (i = 0 ; i<isla_chips; i++){
      chip_isla216p_set_address(fmc2->chip_isla216p[i], i);
      chip_isla216p_soft_reset(fmc2->chip_isla216p[i]);
      fmc2->chip_isla216p[i]->reg_current.cal[0] = 0;

    }
    // get standard cal time
    usleep(chip_isla216p_get_cal_time_ms(fnew) * 1000);
    for (i = 0 ; i<isla_chips; i++){
      int counter = 0;
      //fmc2_isla216p[i]->reg_current.cal[0] = 
      chip_isla216p_registers_download(fmc2->chip_isla216p[i], CHIP_ISLA216P_CAL_ID);
      while( fmc2->chip_isla216p[i]->reg_current.cal[0] != 0x91) {
        printf("Chip[%d] wait for cal bit: %d -> %02X\n", i, counter++,fmc2->chip_isla216p[i]->reg_current.cal[0]);
        usleep(1000);
        chip_isla216p_registers_download(fmc2->chip_isla216p[i], CHIP_ISLA216P_CAL_ID);
      }
      printf("Chip[%d] done after: %d loops\n", i, counter);
      chip_isla216p_registers_download(fmc2->chip_isla216p[i], CHIP_ISLA216P_ALL_ID);
      printf("Chip[%d] Mode: %02X\n", i, fmc2->chip_isla216p[i]->reg_current.adc[5]);
      fmc2->chip_isla216p[i]->reg_current.adc[5] = 0x01;

      fmc2->chip_isla216p[i]->reg_current.out[2] = 0x01;
      fmc2->chip_isla216p[i]->reg_current.out[3] = 0x24;
      if (fnew < 90) {
        fmc2->chip_isla216p[i]->reg_current.out[4] |= 0x40;
      } else {
        fmc2->chip_isla216p[i]->reg_current.out[4] &= 0xBF;
      }
      chip_isla216p_registers_upload(fmc2->chip_isla216p[i], CHIP_ISLA216P_OUT_ID | CHIP_ISLA216P_ADC_ID, NULL );
      chip_isla216p_test_path(fmc2->chip_isla216p[i], ISLA216P_TEST_PATTERN_OFF, 0x0000, 0x0000, 0x0000, 0x0000);
      
      
    }
    
  }
  
  
   }

 return 0;
}
