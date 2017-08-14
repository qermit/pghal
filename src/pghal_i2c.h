#ifndef __SDB_I2C_H___
#define __SDB_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

struct pghal_i2c {
  struct pghal_bus bus; // holds info
  
  int (*write_read)   (struct pghal_i2c *, uint8_t, size_t, uint8_t *, size_t, uint8_t *);
  int (*write)        (struct pghal_i2c *, uint8_t, size_t, char *);
  int (*read)         (struct pghal_i2c *, uint8_t, size_t, char *);
  int (*chip_present) (struct pghal_i2c *, uint8_t);
 
};

int pghal_i2c_write_read(struct pghal_i2c * bus_i2c, uint8_t i2c_address, size_t wr_size, uint8_t * wr_ptr, size_t rd_size, uint8_t * rd_ptr);
int pghal_i2c_chip_present(struct pghal_i2c * bus_i2c, uint8_t i2c_address);

void pghal_i2c_detect(struct pghal_i2c * i2c_bus);

#ifdef __cplusplus
}
#endif

#endif
