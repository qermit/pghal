/******************************************************************************
 * Title      : 
 * Project    : Pretty Good Hardware Abstraction Layer
 ******************************************************************************
 * File       : pghal.c
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

void * pghal_alloc(size_t size) 
{
   void * header = NULL;
   header = malloc(size);
   printf ("pghal_alloc: %p\n", header);
   memset(header, 0, size);
   return header;
}


void pghal_bus_register_new_module(struct pghal_bus * bus, struct list_head * module) {
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

void sdb_module_init(struct sdb_module * handle, struct pghal_bus * bus, uint32_t wb_address)
{
  handle->bus = bus; // @TODO: inc bus usage count
  handle->address = wb_address;

  pghal_bus_register_new_module(bus, &handle->list);
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


void i2c_detect(struct pghal_i2c * i2c_bus)
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

