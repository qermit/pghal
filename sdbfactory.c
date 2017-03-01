#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "list.h"
#include "sdbfactory.h"
#include "sdb_xdma.h"
#include "sdb_i2c.h"

// CHIPS include
#include "chip/chip_si57x.h"

void * libsdb_alloc(size_t size) 
{
   void * header = NULL;
   header = malloc(size);
   printf ("libsdb_alloc: %p\n", header);
   memset(header, 0, size);
   return header;
}


void abs_bus_register_new_module(struct abs_bus * bus, struct list_head * module) {
  list_add_tail(module, &bus->module_list);
}

/*
struct sdbbus * fmc_sdbbus_alloc(struct sdbbus * handle)
{
}


void       fmc_sdbbus_init(struct sdbbus * handle, char * address)
{
}


void       fmc_sdbbus_destroy(struct sdbbus * handle)
{
}


*/

/*
struct fmc_dio5 * fmc_dio5_alloc(struct fmc_dio5 * handle, struct sdbbus * bus)
{
}

void     fmc_dio5_init(struct fmc_dio5 * handle, uint32_t address) 
{
}

void     fmc_dio5_destroy(struct fmc_dio5 * handle) 
{
}
*/

void sdb_module_init(struct sdb_module * handle, struct abs_bus * bus, uint32_t wb_address)
{
  handle->bus = bus; // @TODO: inc bus usage count
  handle->address = wb_address;

  abs_bus_register_new_module(bus, &handle->list);
}

/*
struct gpio_raw * gpio_raw_init(struct gpio_raw * handle, struct sdbbus * bus, uint32_t wb_address)
{
  struct gpio_raw * gpio = handle;
  if (gpio == NULL) {
    gpio = (struct gpio_raw *) libsdb_alloc((libsdb_header *) gpio, sizeof(struct gpio_raw));
    wb_module_init(&gpio->wb, bus, wb_address);
  }
  return gpio;
}

struct wb_spi * wb_spi_init(struct wb_spi * handle, struct sdbbus * bus, uint32_t wb_address)
{
  struct wb_spi * spi = handle;
  if (spi == NULL) {
    spi = (struct wb_spi *) libsdb_alloc((libsdb_header *) spi, sizeof(struct wb_spi));
    wb_module_init(&spi->wb, bus, wb_address);
  }
  return spi;
}
*/


/*
adc250_t * fmc_adc250_alloc(adc250_t * handle, struct sdbbus * bus)
{
  adc250_t * adc250 = handle;
  if (adc250 == NULL) {
    adc250 = (adc250_t *) libsdb_alloc(NULL,sizeof(adc250_t));
    if (adc250 == NULL) { } // TODO: check if valid alocated
  }
  handle->wb.bus = bus;
  handle->wb.wb_address = 0;
  return adc250;
}

// | +-- 0x00004000 - 0x00005FFF: fmc-adc-250m-16b (sdb@0x00004400)
// | +-- 0x00004000 - 0x0000401F: SPI.AD9510
// | +-- 0x00004020 - 0x0000403F: SPI.ISLA216P
// | +-- 0x00004040 - 0x0000405F: SPI.AMC7823
// | +-- 0x00004100 - 0x000041FF: I2C.Si57x
// | +-- 0x00004200 - 0x000042FF: WB-GPIO-RAW
// | +-- 0x00005000 - 0x00005FFF: IODELAY CTL

adc250_t * fmc_adc250_init(adc250_t * handle, uint32_t address) 
{
   wb_module_init(&handle->wb, handle->wb.bus, address);

   handle->gpio_bus = gpio_raw_init((struct gpio_raw *) NULL, handle->wb.bus, address + 0x200);
   handle->vcxo_bus = wb_i2c_init  ((struct wb_i2c *) NULL,   handle->wb.bus, address + 0x100);

//   handle->vcxo = address + 0x100
//   handlespi_t * pll;
//  spi_t * adc;
//  spi_t * mon;
//  gpio_raw_t * gpio;
//   handle->
}

void       fmc_adc250_destroy(adc250_t * handle) 
{
}

struct chip_si57x  *  si57x_init(struct wb_i2c * i2c_bus, uint8_t i2c_addr) {
}


void fmc_adc250_enable_vcxo(adc250_t * handle, int enable) {
}

*/


void i2c_detect(struct abs_i2c * i2c_bus)
{

 int res;
 uint8_t chip_id = 0x49;


printf("Probing available I2C devices using bus  ...");
printf("\n     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
printf("\n====================================================");
for(chip_id = 0x0; chip_id < 128 ; chip_id++) {

if ( (chip_id % 16) == 0 ) {
        printf("\n%02X  ", chip_id/16);
 }
  if ( chip_id <= 7 || chip_id > 120 ) {
    printf("   ");
    continue;
  }

  res = i2c_bus->chip_present(i2c_bus, chip_id);

  if ( res == 0 ) {
    printf(" %02X", chip_id);
  } else {
    printf(" --",res);
  }
 }
 printf("\n");

}

int main(){
 struct bus_xdma * xdma = NULL;
 xdma = xdma_open_bus("/dev/xdma/card0/user");

 struct abs_bus * bus = &xdma->bus;

 
 struct wb_i2c * si57x_i2c_bus = wb_i2c_init(bus ,0x00004100);
 //i2c_detect(&si57x_i2c_bus->i2c);

 unsigned char data_to_write[] = { 0x07 };
 unsigned char data_to_read[12];
 /*
 si57x_i2c_bus->i2c.write_read(&si57x_i2c_bus->i2c, 0x49, 1, data_to_write, 12, data_to_read);
 int i;
 for(i = 0; i< 12 ; i++){
   printf("vcxo[%02d]: 0x%02X\n", i+7, data_to_read[i]);
 }
 */

 struct chip_si57x * fmc2_si57x = chip_si57x_init(&si57x_i2c_bus->i2c);
 chip_si57x_set_from_part_number(fmc2_si57x, "571AJC000337DG");
// chip_si57x_set_address(fmc2_si57x, 0x48);
 chip_si57x_reload_initial(fmc2_si57x);

 fmc2_si57x->reg_current.hsdiv = fmc2_si57x->reg_init.hsdiv;
 fmc2_si57x->reg_current.n1    = fmc2_si57x->reg_init.n1;
 fmc2_si57x->reg_current.rfreq = fmc2_si57x->reg_init.rfreq;
 
 { int i;
  for (i = 0; i<6 ; i++) { 
    printf("DATA[%d] = 0x%02X vs 0x%02X\n", i+7, fmc2_si57x->reg_init.regs_raw[i], fmc2_si57x->reg_current.regs_raw[i]);
  }
 }

 chip_si57x_val_to_regs(&fmc2_si57x->reg_current);
 { int i;
  for (i = 0; i<6 ; i++) { 
    printf("DATA[%d] = 0x%02X vs 0x%02X\n", i+7, fmc2_si57x->reg_init.regs_raw[i], fmc2_si57x->reg_current.regs_raw[i]);
  }
 }
  
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
