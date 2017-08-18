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

#include "pghal_inc.h"

#include "sdb_xdma.h"
//#include "sdb_i2c.h"

// CHIPS include
//#include "chip/chip_si57x.h"

void * pghal_alloc(size_t size) 
{
   void * header = NULL;
   header = malloc(size);
   memset(header, 0, size);
   return header;
}

void pghal_dump_regs(uint8_t *ptr, size_t len)
{
  int tmp_i;
  printf("      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
  for (tmp_i = 0; tmp_i < len; tmp_i += 1) {
   if ( tmp_i % 16 == 0) printf("\n%2X0:", tmp_i / 16);
   printf (" %02x", ptr[tmp_i]);
  }

  printf ("\n");
}

void pghal_node_driver_register( struct pghal_list *list, struct pghal_node_driver * driver)
{
  list_add_tail(&driver->list, list);
}

void pghal_bus_driver_register( struct pghal_list *list, struct pghal_bus_driver * driver)
{
  list_add_tail(&driver->list, list);
}


void pghal_bus_register_new_module(struct pghal_bus * bus, struct pghal_list * module) {
  list_add_tail(module, &bus->module_list);
}


size_t pghal_bus_write(struct pghal_bus * bus, struct pghal_address * addr, size_t wr_len, void *wr_ptr )
{
  // @TODO: check if driver valid
  if (bus == NULL) return 0;
  struct pghal_bus_driver * driver = bus->driver;
  // no driver ???
  if (driver == NULL){
    return 0;
  }
  if (driver->op.write == NULL){
    return 0;
  }
  return  driver->op.write(bus, addr, wr_len, wr_ptr);
}

size_t pghal_bus_read(struct pghal_bus *bus, struct pghal_address * addr, size_t rd_len, void *rd_ptr)
{
  if (bus == NULL) return 0;
  struct pghal_bus_driver * driver = bus->driver;
  // no driver ???
  if (driver == NULL){
    return 0;
  }
  if (driver->op.read == NULL){
    printf("no read handler\n");
    return 0;
  }
  
  return  driver->op.read(bus, addr, rd_len, rd_ptr);
//  return 0;
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


