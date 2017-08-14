#ifndef __WB_I2C_H__
#define __WB_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

struct wb_i2c {
  struct sdb_module sdb;
  struct pghal_i2c  i2c;

  struct sdb_node_address tmp_address;
};


struct wb_i2c * wb_i2c_init(struct pghal_bus * bus, uint32_t wb_address);


#ifdef __cplusplus
}
#endif

#endif
