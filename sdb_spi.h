#ifndef __SDB_SPI_H___
#define __SDB_SPI_H__




struct pghal_spi {
  struct pghal_bus bus; // spi abstraction bus exported to SPI chips
  
  int (*write_read)   (struct pghal_spi *, uint8_t , size_t, char *, size_t, char *);
  int (*write)        (struct pghal_spi *, uint8_t, size_t, char *);
  int (*read)         (struct pghal_spi *, uint8_t, size_t, char *);
  int (*chip_present) (struct pghal_spi *, uint8_t);
 
};

void pghal_spi_bus_init(struct pghal_spi *spi);
int pghal_spi_write_read(struct pghal_spi * bus_spi, uint8_t spi_address, size_t wr_size, char * wr_ptr, size_t rd_size, char * rd_ptr);





//ruct wb_spi * wb_spi_init(struct pghal_bus * bus, uint32_t wb_address);


#endif
