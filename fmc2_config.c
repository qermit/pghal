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

#include "pghal_spi.h"
#include "wishbone/wb_spi.h"

#include "pghal_i2c.h"
#include "wishbone/wb_i2c.h"

#include "wishbone/wb_gpio_raw.h"

// CHIPS include
#include "chip/chip_ad9510.h"
#include "chip/chip_si57x.h"
#include "chip/chip_isla216p.h"


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

  int fmc_id = 1;
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
  
  struct xdma_node * xdma = NULL;
  xdma = xdma_open_bus("/dev/xdma/card0/user");

//  struct wb_gpio_raw * fmc1_gpio = wb_gpio_raw_create_direct(&xdma->bus, OFFSET_FMC1 | OFFSET_DIO5_GPIO);
  struct wb_gpio_raw * fmc2_gpio = wb_gpio_raw_create_direct(&xdma->bus, OFFSET_FMC | OFFSET_ADC250_GPIO);
  wb_gpio_raw_set_port_direction(fmc2_gpio, 0x08); 
  wb_gpio_raw_set_port_value(fmc2_gpio, 0x8, 0x8); 

  // debug spi and i2c over FMC1 dio;
  // it's bitstream specific
//  wb_gpio_raw_set_port_altf(fmc1_gpio, 0b00000110); 
//  wb_gpio_raw_set_port_direction(fmc1_gpio, 0b00000110); 
//  wb_gpio_raw_set_port_termination(fmc1_gpio, 0b00000001); 


  double fnew = 100.0;
  struct wb_i2c * si57x_i2c_bus = wb_i2c_init(&xdma->bus , OFFSET_FMC | OFFSET_ADC250_I2C_SI57X);
  struct chip_si57x * fmc2_si57x = chip_si57x_init(&si57x_i2c_bus->i2c);

  if (1) { 
  if (0) {
    chip_si57x_decode_part_number(fmc2_si57x, "TESTTESTTEST01");
    fnew = 161.132812;
  } else {
    chip_si57x_decode_part_number(fmc2_si57x, "571AJC000337DG");
    fnew = 125.0;
  }  

  chip_si57x_reload_initial(fmc2_si57x);

  chip_si57x_find_valid_combo(fmc2_si57x, fnew, SI57X_METHOD_HARD);
  chip_si57x_send_regs(fmc2_si57x, &fmc2_si57x->reg_new, SI57X_METHOD_HARD);
  chip_si57x_registers_download(fmc2_si57x);

  // enbable VCXO and PLL
  wb_gpio_raw_set_port_value(fmc2_gpio, 0x00, 0x20); 
//  wb_gpio_raw_set_port_altf(fmc2_gpio, 0x8); 
  wb_gpio_raw_set_port_altf(fmc2_gpio, 0x0);  // <- reset all alternative functions
  usleep(1);
  wb_gpio_raw_set_port_value(fmc2_gpio, 0x2000 | 0x20, 0x2000 | 0x20); 
  //wb_gpio_raw_set_port_direction(fmc2_gpio, 0x2000 | 0x20 | 0x08); // 0x08: trigger
  wb_gpio_raw_set_port_direction(fmc2_gpio, 0x2000 | 0x20);
  }
  if (1) {
  // configure AD9510
  struct wb_spi * spi_ad9510   = wb_spi_init(&xdma->bus, OFFSET_FMC | OFFSET_ADC250_SPI_AD9510);
  struct chip_ad9510 * chip_ad9510 = chip_ad9510_init(&spi_ad9510->spi);

  chip_ad9510_set_address(chip_ad9510, 0);
  chip_ad9510_soft_reset(chip_ad9510);

//  return 0;
  chip_ad9510_registers_download(chip_ad9510, 0, 100);
  chip_ad9510_config1(chip_ad9510);
  chip_ad9510_registers_download(chip_ad9510, 0, 100); // donload to assert;
  chip_ad9510_registers_commit(chip_ad9510);
  
  wb_gpio_raw_set_port_value(fmc2_gpio, 0, 0x8); 
  }
  //configure 4x ADC
  struct wb_spi * spi_isla216p = wb_spi_init(&xdma->bus, OFFSET_FMC | OFFSET_ADC250_SPI_ISLA);
  if (1) {
  // assert and deassert ADC resets
  wb_gpio_raw_set_port_value(fmc2_gpio, 0x0000, 0x0400);
  usleep(1000);
  wb_gpio_raw_set_port_value(fmc2_gpio, 0x0400, 0x0400);
  usleep(1000);


  int isla_chips = 4;
  struct chip_isla216p * fmc2_isla216p[4];
  // reset first, wait for all in separate loop
  { int i;
    for (i = 0 ; i<isla_chips; i++){
      fmc2_isla216p[i] = chip_isla216p_init(&spi_isla216p->spi);
      chip_isla216p_set_address(fmc2_isla216p[i], i);
      chip_isla216p_soft_reset(fmc2_isla216p[i]);
      fmc2_isla216p[i]->reg_current.cal[0] = 0;

    }
    // get standard cal time
    usleep(chip_isla216p_get_cal_time_ms(fnew) * 1000);
    for (i = 0 ; i<isla_chips; i++){
      int counter = 0;
      //fmc2_isla216p[i]->reg_current.cal[0] = 
      chip_isla216p_registers_download(fmc2_isla216p[i], CHIP_ISLA216P_CAL_ID);
      while( fmc2_isla216p[i]->reg_current.cal[0] != 0x91) {
        printf("Chip[%d] wait for cal bit: %d -> %02X\n", i, counter++,fmc2_isla216p[i]->reg_current.cal[0]);
        usleep(1000);
        chip_isla216p_registers_download(fmc2_isla216p[i], CHIP_ISLA216P_CAL_ID);
      }
      printf("Chip[%d] done after: %d loops\n", i, counter);
      chip_isla216p_registers_download(fmc2_isla216p[i], CHIP_ISLA216P_ALL_ID);
      printf("Chip[%d] Mode: %02X\n", i, fmc2_isla216p[i]->reg_current.adc[5]);
      fmc2_isla216p[i]->reg_current.adc[5] = 0x01;

      fmc2_isla216p[i]->reg_current.out[2] = 0x01;
      fmc2_isla216p[i]->reg_current.out[3] = 0x24;
      if (fnew < 90) {
        fmc2_isla216p[i]->reg_current.out[4] |= 0x40;
      } else {
        fmc2_isla216p[i]->reg_current.out[4] &= 0xBF;
      }
      chip_isla216p_registers_upload(fmc2_isla216p[i], CHIP_ISLA216P_OUT_ID | CHIP_ISLA216P_ADC_ID, NULL );
      chip_isla216p_test_path(fmc2_isla216p[i], ISLA216P_TEST_PATTERN_OFF, 0x0000, 0x0000, 0x0000, 0x0000);
      
      
    }
    
  }
  
  
   }

  
  return 0;
  struct wb_spi * spi_amc7823  = wb_spi_init(&xdma->bus, 0x00004040);
  //printf("Chip presetn: %d\n",  pghal_i2c_chip_present(&si57x_i2c_bus->i2c, 0x48));
  
  pghal_i2c_detect(&si57x_i2c_bus->i2c);
  return 0;

  uint32_t addr = 0x200;
  printf("xdma_read32 : 0x%08X <- 0x%08X\n", addr, xdma_read_direct(xdma, addr));
  printf("method xdma_write_direct\n");
  int i;
  for(i = 0; i<10; i++) {
    xdma_write_direct(xdma, 0x00004204, 1 << 3 | 1);
    usleep(1000 * 100);
    xdma_write_direct(xdma, 0x00004200, 1 << 3 | 1);
    usleep(1000 * 100);
  }
  usleep(1000 * 1000);
  printf("method pghal_bus_write\n");
  struct sdb_node_address * sdb_addr_set = sdb_address_create(&xdma->bus, 0x4204);
  struct sdb_node_address * sdb_addr_clr = sdb_address_create(&xdma->bus, 0x4200);
  uint32_t data[1] = { 1<<3 | 2 };
  for(i = 0; i<10; i++) {
    pghal_bus_write(&xdma->bus, &sdb_addr_set->address, 1*sizeof(uint32_t), data);
    usleep(1000 * 100);
    pghal_bus_write(&xdma->bus, &sdb_addr_clr->address, 1*sizeof(uint32_t), data);
    usleep(1000 * 100);
  }
   
 
  usleep(1000 * 1000);
  printf("method wb_gpio\n");

  for(i = 0; i<10; i++) {
    wb_gpio_raw_set_port_value(fmc2_gpio, 1 << 3 | 4, 1 << 3 | 4);
    usleep(1000 * 100);
    wb_gpio_raw_set_port_value(fmc2_gpio, 0, 1 << 3 | 4);
    usleep(1000 * 100);
  }
 
 return 0;
}
/*
 struct pghal_bus * bus = &xdma->bus;
 
 struct wb_gpio_raw * fmc2_gpio = wb_gpio_raw_init(bus ,0x00004200);

 wb_gpio_raw_set_port_direction(fmc2_gpio, 1 << 3);

 int i;
 for(i = 0; i<10; i++) {
   wb_gpio_raw_set_port_value(fmc2_gpio, 1 << 3 | 1, 1 << 3 | 1);
   usleep(1000 * 100);
   wb_gpio_raw_set_port_value(fmc2_gpio, 0, 1 << 3 | 1);
   usleep(1000 * 100);
 }

 

 return;
 struct wb_i2c * si57x_i2c_bus = wb_i2c_init(bus ,0x00004100);
 pghal_i2c_detect(&si57x_i2c_bus->i2c);


 struct chip_si57x * fmc2_si57x = chip_si57x_init(&si57x_i2c_bus->i2c);
 chip_si57x_decode_part_number(fmc2_si57x, "571AJC000337DG");
 chip_si57x_reload_initial(fmc2_si57x);

 double fnew = chip_si57x_get_fout0(fmc2_si57x);
 fnew = 125.0;

 chip_si57x_find_valid_combo(fmc2_si57x, fnew, SI57X_METHOD_HARD);
 chip_si57x_send_regs(fmc2_si57x, &fmc2_si57x->reg_new, SI57X_METHOD_HARD);
 
// double fold = chip_si57x_get_fout0(fmc2_si57x);
// printf("Current PPM change %lf MHz -> %lf MHz: %.3lf ppm\n", fold, fnew, chip_si57x_get_ppm(fnew, fold));

//  wb_gpio_raw_set_port_value(fmc2_gpio, uint32_t term, uint32_t mask); fmc2_gpio
 
 return 0;

 printf("bus->head.prev: %p, next: %p\n", bus->module_list.prev, bus->module_list.next);
 struct wb_i2c * si57x_i2c_bus2 = wb_i2c_init(bus ,0x00004200);

 printf("bus->head.prev: %p, next: %p\n", bus->module_list.prev, bus->module_list.next);
 
 printf("si57x_i2c_bus : %p\n", &si57x_i2c_bus->sdb.list);
 printf("si57x_i2c_bus2: %p\n", &si57x_i2c_bus2->sdb.list);

 //printf("bus_xdma: %p\n", bus->bus_address);
//  asm("int3");
 //printf("bus_address: %s\n", bus->bus_address);
 //printf("bus_op_read: %p\n", xdma->header.op_read);
 
 printf("BUS_READ 0x200 -> 0x%08X\n", bus->read(bus, 0x200));
 bus->write(bus, 0x4, 6);
 printf("BUS_READ 0x2200 -> 0x%08X\n", bus->read(bus, 0x2200));
 bus->write(bus, 0x4204, 1);
*/
//return 0;
//}

