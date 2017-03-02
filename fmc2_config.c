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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "list.h"
#include "pghal.h"
#include "sdb_xdma.h"
#include "sdb_i2c.h"

// CHIPS include
#include "chip/chip_si57x.h"

int main( int argc, char** argv){
 struct bus_xdma * xdma = NULL;
 xdma = xdma_open_bus("/dev/xdma/card0/user");

 struct pghal_bus * bus = &xdma->bus;

 
 struct wb_i2c * si57x_i2c_bus = wb_i2c_init(bus ,0x00004100);
 //i2c_detect(&si57x_i2c_bus->i2c);


 struct chip_si57x * fmc2_si57x = chip_si57x_init(&si57x_i2c_bus->i2c);
 chip_si57x_decode_part_number(fmc2_si57x, "571AJC000337DG");
 chip_si57x_reload_initial(fmc2_si57x);

 double fnew = chip_si57x_get_fout0(fmc2_si57x);
 fnew = 125.0;

 chip_si57x_find_valid_combo(fmc2_si57x, fnew, SI57X_METHOD_HARD);
 chip_si57x_send_regs(fmc2_si57x, &fmc2_si57x->reg_new, SI57X_METHOD_HARD);
 
// double fold = chip_si57x_get_fout0(fmc2_si57x);
// printf("Current PPM change %lf MHz -> %lf MHz: %.3lf ppm\n", fold, fnew, chip_si57x_get_ppm(fnew, fold));
 
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
return 0;
}
