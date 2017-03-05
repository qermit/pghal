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
#include "pghal_i2c.h"

#include "wishbone/wb_i2c.h"
#include "wishbone/wb_gpio_raw.h"

// CHIPS include
//#include "chip/chip_si57x.h"

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);

int main( int argc, char** argv){
  
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);

  struct xdma_node * xdma = NULL;
  xdma = xdma_open_bus("/dev/xdma/card0/user");
  printf("XDMA bus driver %p\n", xdma->bus.driver);


  struct wb_i2c * si57x_i2c_bus = wb_i2c_init(&xdma->bus ,0x00004100);
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
   
 
  struct wb_gpio_raw * fmc2_gpio = wb_gpio_raw_create_direct(&xdma->bus, 0x4200);
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

