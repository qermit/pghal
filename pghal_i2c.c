#include "pghal_inc.h"
#include "sdb_bus.h"
#include "pghal_i2c.h"

#define WB_I2C_REG_PERR_LOW  0x00
#define WB_I2C_REG_PERR_HIGH 0x04
#define WB_I2C_REG_CTR       0x08
#define WB_I2C_REG_RXR_TXR   0x0C
#define WB_I2C_REG_SR_CR     0x10
#define WB_I2C_REG_IFACE     0x14
#define WB_I2C_REG_TXR_DBG   0x18
#define WB_I2C_REG_CR_DBG    0x1C

#define WB_I2C_CTR_CORE_EN 0x00000080
#define WB_I2C_CTR_IEN     0x00000040


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




// errors -> -1 arbitration lost
//        -> -2 no ack
static int wb_i2c_write_read(struct pghal_i2c * handle, uint8_t i2c_address, size_t wr_size, uint8_t * wr_ptr, size_t rd_size, uint8_t * rd_ptr)
{
  uint32_t FLAG;
  int i;

  struct wb_i2c * wb_i2c = (struct wb_i2c *) (((void *) handle) - offsetof(struct wb_i2c, i2c));
  struct pghal_bus * bus = wb_i2c->sdb.bus;
  uint32_t wb_address = wb_i2c->sdb.address.sdb_address;
  struct sdb_node_address * tmp_address = &wb_i2c->tmp_address;
  uint32_t data_w;
  uint32_t SR;

  tmp_address->sdb_address = wb_address + WB_I2C_REG_SR_CR ; pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);

  if ((wr_size > 0) || (wr_size == 0 && rd_size == 0)) {
    tmp_address->sdb_address = wb_address + WB_I2C_REG_RXR_TXR ; data_w = i2c_addr_w(i2c_address); pghal_bus_write(bus, &tmp_address->address , 1*sizeof(uint32_t), &data_w);
    tmp_address->sdb_address = wb_address + WB_I2C_REG_SR_CR   ; data_w = 0b10010000             ; pghal_bus_write(bus, &tmp_address->address , 1*sizeof(uint32_t), &data_w);

    tmp_address->sdb_address = wb_address + WB_I2C_REG_SR_CR ; pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);
    while (SR & 0x00000002) {
      pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);
      if (SR & 0x20 ) return -1;
    }

    uint8_t * wr_ptr8 = wr_ptr; // todo <- conversion from void *
    for(i=0; i<wr_size; i++) {
      tmp_address->sdb_address = wb_address + WB_I2C_REG_RXR_TXR ; data_w = wr_ptr8[i]  ; pghal_bus_write(bus, &tmp_address->address , 1*sizeof(uint32_t), &data_w);
      tmp_address->sdb_address = wb_address + WB_I2C_REG_SR_CR   ; data_w = 0b00010000  ; pghal_bus_write(bus, &tmp_address->address , 1*sizeof(uint32_t), &data_w);

      pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);
      while (SR & 0x00000002) {
        pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);
        if (SR & 0x20 ) return -1;
      }
    }
  }

  if (rd_size > 0) {
    tmp_address->sdb_address = wb_address + WB_I2C_REG_RXR_TXR ; data_w = i2c_addr_r(i2c_address); pghal_bus_write(bus, &tmp_address->address , 1*sizeof(uint32_t), &data_w);
    tmp_address->sdb_address = wb_address + WB_I2C_REG_SR_CR   ; data_w = 0b10010000             ; pghal_bus_write(bus, &tmp_address->address , 1*sizeof(uint32_t), &data_w);

    pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);
    while (SR & 0x00000002) {
      pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);
    }

    for(i=0; i<rd_size; i++) {
      tmp_address->sdb_address = wb_address + WB_I2C_REG_SR_CR   ; pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);
      FLAG = 0b00100000;

      if (i == rd_size - 1)
        FLAG = 0b01101000; // # Finish and N ack to finish transfer

      tmp_address->sdb_address = wb_address + WB_I2C_REG_SR_CR   ; pghal_bus_write(bus, &tmp_address->address , 1*sizeof(uint32_t), &FLAG);

      tmp_address->sdb_address = wb_address + WB_I2C_REG_SR_CR   ; pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);
      while (SR & 0x00000002) {
        pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);
      }
      tmp_address->sdb_address = wb_address + WB_I2C_REG_RXR_TXR ; pghal_bus_write(bus, &tmp_address->address , 1*sizeof(uint32_t), &data_w);
      rd_ptr[i] = data_w;
    }
  } else {
    tmp_address->sdb_address = wb_address + WB_I2C_REG_SR_CR   ; data_w = 0b01000000   ; pghal_bus_write(bus, &tmp_address->address , 1*sizeof(uint32_t), &data_w);
  }
  tmp_address->sdb_address = wb_address + WB_I2C_REG_SR_CR   ; pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);
  while (SR & 0x00000040) {
    pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);
  }

  if (SR & 0x80) return -2;
//  if (SR & 0x20 ) return -1;

  return 0;
}

static int wb_i2c_detect(struct pghal_i2c * handle, uint8_t i2c_address)
{
//  printf("wb_i2c_write_read: check: %02x\n", i2c_address);
  return wb_i2c_write_read(handle, i2c_address, 0, NULL, 0, NULL);
}

struct wb_i2c * wb_i2c_init(struct pghal_bus * bus, uint32_t wb_address)
{
  struct wb_i2c * i2c = NULL;
  i2c = (struct wb_i2c *) pghal_alloc((sizeof(struct wb_i2c)));

  sdb_node_init(&i2c->sdb, bus, wb_address);

  i2c_bus_init(&i2c->i2c);

  i2c->i2c.write_read = wb_i2c_write_read;
  i2c->i2c.chip_present = wb_i2c_detect;

 
//  pghal_i2c_write_read(struct pghal_i2c * bus_i2c, uint8_t i2c_address, size_t wr_size, char * wr_ptr, size_t rd_size, char * rd_ptr);
//  pghal
 
//  i2c->sdb.bus->write(bus, wb_address + WB_I2C_REG_PERR_HIGH, 0);
//  i2c->sdb.bus->write(bus, wb_address + WB_I2C_REG_PERR_LOW, 253);
//  i2c->sdb.bus->write(bus, wb_address + WB_I2C_REG_CTR, WB_I2C_CTR_CORE_EN);


  return i2c;
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

  res = i2c_bus->chip_present(i2c_bus, chip_id);

  if ( res == 0 ) {
    printf(" %02X", chip_id);
  } else {
    printf(" --",res);
  }
 }
 printf("\n");

}


