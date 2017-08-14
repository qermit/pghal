#include "pghal_inc.h"
#include "sdb_bus.h"
#include "pghal_i2c.h"
#include "wb_i2c.h"

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

  struct sdb_node_address reg_sr_cr; memcpy(&reg_sr_cr, &wb_i2c->sdb.address, sizeof(struct sdb_node_address)); reg_sr_cr.sdb_address = reg_sr_cr.sdb_address + WB_I2C_REG_SR_CR;
  struct sdb_node_address reg_rxr_txr; memcpy(&reg_rxr_txr, &wb_i2c->sdb.address, sizeof(struct sdb_node_address)); reg_rxr_txr.sdb_address = reg_rxr_txr.sdb_address + WB_I2C_REG_RXR_TXR;


  uint32_t data_w;
  uint32_t SR;
  uint32_t CR;

  pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);

  if ((wr_size > 0) || (wr_size == 0 && rd_size == 0)) {
    data_w = i2c_addr_w(i2c_address);
    pghal_bus_write(bus, &reg_rxr_txr.address, sizeof(uint32_t), &data_w);
    CR = 0b10010000;
    pghal_bus_write(bus, &reg_sr_cr.address, sizeof(uint32_t), &CR);    

    pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);
    while (SR & 0x00000002) {
      pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);
      if (SR & 0x20 ) return -1;
    }

    uint8_t * wr_ptr8 = wr_ptr; // todo <- conversion from void *
    for(i=0; i<wr_size; i++) {
      data_w = wr_ptr8[i];
      pghal_bus_write(bus, &reg_rxr_txr.address , 1*sizeof(uint32_t), &data_w);
      CR = 0b00010000;
      pghal_bus_write(bus, &reg_sr_cr.address, sizeof(uint32_t), &CR);    

      pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);
      while (SR & 0x00000002) {
        pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);
        if (SR & 0x20 ) return -1;
      }
    }
  }

  if (rd_size > 0) {
    data_w = i2c_addr_r(i2c_address);
    pghal_bus_write(bus, &reg_rxr_txr.address, sizeof(uint32_t), &data_w);
    CR = 0b10010000;
    pghal_bus_write(bus, &reg_sr_cr.address, sizeof(uint32_t), &CR);    

    pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);
    while (SR & 0x00000002) {
      pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);
      if (SR & 0x20 ) return -1;
    }

    for(i=0; i<rd_size; i++) {
      pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);
      CR = 0b00100000;

      if (i == rd_size - 1)
        CR = 0b01101000; // # Finish and N ack to finish transfer

      pghal_bus_write(bus, &reg_sr_cr.address, sizeof(uint32_t), &CR);    

      pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);
      while (SR & 0x00000002) {
        pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);
        if (SR & 0x20 ) return -1;
      }
      
      pghal_bus_read(bus, &reg_rxr_txr.address, sizeof(uint32_t), &data_w);
      rd_ptr[i] = data_w;
    }
  } else {
    CR = 0b01000000;
    pghal_bus_write(bus, &reg_sr_cr.address, sizeof(uint32_t), &CR);
  }

  pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);
  while (SR & 0x00000040) {
    pghal_bus_read(bus, &reg_sr_cr.address, sizeof(uint32_t), &SR);
  }

  if (SR & 0x80) return -2;
//  if (SR & 0x20 ) return -1;

  return 0;
}

static int wb_i2c_detect(struct pghal_i2c * i2c, uint8_t i2c_address)
{
  return wb_i2c_write_read(i2c, i2c_address, 0, NULL, 0, NULL);
}

struct wb_i2c * wb_i2c_init(struct pghal_bus * bus, uint32_t wb_address)
{
  struct wb_i2c * wb_i2c = NULL;
  wb_i2c = (struct wb_i2c *) pghal_alloc((sizeof(struct wb_i2c)));

  sdb_node_init(&wb_i2c->sdb, bus, wb_address);

  i2c_bus_init(&wb_i2c->i2c);

  wb_i2c->i2c.write_read = wb_i2c_write_read;
  wb_i2c->i2c.chip_present = wb_i2c_detect;

 
//  pghal_i2c_write_read(struct pghal_i2c * bus_i2c, uint8_t i2c_address, size_t wr_size, char * wr_ptr, size_t rd_size, char * rd_ptr);
//  pghal
  struct sdb_node_address reg_perr_high; memcpy(&reg_perr_high, &wb_i2c->sdb.address, sizeof(struct sdb_node_address)); reg_perr_high.sdb_address = reg_perr_high.sdb_address + WB_I2C_REG_PERR_HIGH;
  struct sdb_node_address reg_perr_low; memcpy(&reg_perr_low, &wb_i2c->sdb.address, sizeof(struct sdb_node_address)); reg_perr_low.sdb_address = reg_perr_low.sdb_address + WB_I2C_REG_PERR_LOW;
  struct sdb_node_address reg_ctr; memcpy(&reg_ctr, &wb_i2c->sdb.address, sizeof(struct sdb_node_address)); reg_ctr.sdb_address = reg_ctr.sdb_address + WB_I2C_REG_CTR;
 
  uint32_t data_w = 0;
  pghal_bus_write(bus, &reg_perr_high.address, sizeof(uint32_t), &data_w);
  data_w = 253;
  pghal_bus_write(bus, &reg_perr_low.address, sizeof(uint32_t), &data_w);
  data_w = WB_I2C_CTR_CORE_EN;
  pghal_bus_write(bus, &reg_ctr.address, sizeof(uint32_t), &data_w);
//  i2c->sdb.bus->write(bus, wb_address + WB_I2C_REG_PERR_HIGH, 0);
//  i2c->sdb.bus->write(bus, wb_address + WB_I2C_REG_PERR_LOW, 253);
//  i2c->sdb.bus->write(bus, wb_address + WB_I2C_REG_CTR, WB_I2C_CTR_CORE_EN);


  return wb_i2c;
}


