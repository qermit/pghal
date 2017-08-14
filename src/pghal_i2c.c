#include "pghal_inc.h"
#include "sdb_bus.h"
#include "pghal_i2c.h"


inline uint8_t i2c_addr_r(uint8_t addr_7bit)
{
  return ((addr_7bit << 1) | 1);
}
inline uint8_t i2c_addr_w(uint8_t addr_7bit)
{
  return ((addr_7bit << 1));
}


void i2c_bus_init(struct pghal_i2c *i2c)
{
  struct pghal_bus * bus = & i2c->bus;
  //bus->read  = xdma_read32;
  //bus->write = xdma_write32;
  
  INIT_LIST_HEAD(&bus->module_list);
}

void i2c_chip_register(struct pghal_i2c *i2c, uint8_t i2c_address)
{
  
}

int pghal_i2c_write_read(struct pghal_i2c * bus_i2c, uint8_t i2c_address, size_t wr_size, uint8_t * wr_ptr, size_t rd_size, uint8_t * rd_ptr) {
  return bus_i2c->write_read(bus_i2c, i2c_address, wr_size, wr_ptr, rd_size, rd_ptr);
}

int pghal_i2c_chip_present(struct pghal_i2c * bus_i2c, uint8_t i2c_address) {
  return bus_i2c->chip_present(bus_i2c, i2c_address);
}

void pghal_i2c_detect(struct pghal_i2c * i2c_bus)
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

  res = pghal_i2c_chip_present(i2c_bus, chip_id);

  if ( res == 0 ) {
    printf(" %02X", chip_id);
  } else {
    printf(" --",res);
  }
 }
 printf("\n");

}


