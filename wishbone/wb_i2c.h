#ifndef __WB_I2C_H___
#define __WB_I2C_H__

struct wb_i2c {
  struct sdb_module sdb;
  struct pghal_i2c  i2c;

  struct sdb_node_address tmp_address;
};


struct wb_i2c * wb_i2c_init(struct pghal_bus * bus, uint32_t wb_address);


#endif
