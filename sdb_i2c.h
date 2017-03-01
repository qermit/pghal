#ifndef __SDB_I2C_H___
#define __SDB_I2C_H__




struct abs_i2c {
  struct abs_bus bus;
  
  int (*write_read)   (struct abs_i2c *, uint8_t , size_t, char *, size_t, char *);
  int (*write)        (struct abs_i2c *, uint8_t, size_t, char *);
  int (*read)         (struct abs_i2c *, uint8_t, size_t, char *);
  int (*chip_present) (struct abs_i2c *, uint8_t);
 
};

int abs_i2c_write_read(struct abs_i2c * bus_i2c, uint8_t i2c_address, size_t wr_size, char * wr_ptr, size_t rd_size, char * rd_ptr);



struct wb_i2c {
  struct sdb_module sdb;
  struct abs_i2c    i2c;
};


struct wb_i2c * wb_i2c_init(struct abs_bus * bus, uint32_t wb_address);


#endif
