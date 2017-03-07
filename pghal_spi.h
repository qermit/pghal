#ifndef __SDB_SPI_H___
#define __SDB_SPI_H__


struct pghal_spi_address {
  struct pghal_address address; 

  uint32_t ss_id;
};


struct pghal_spi {
  struct pghal_bus bus; // spi abstraction bus exported to SPI chips
  
  int (*write_read)   (struct pghal_spi * spi, uint8_t ss_id, struct pghal_op_rw * op);
  int (*a16write)     (struct pghal_spi * spi, uint8_t, size_t, char *);
  int (*a16read)      (struct pghal_spi * spi, uint8_t, size_t, char *);
  int (*chip_present) (struct pghal_spi * spi, uint8_t);
};

void pghal_spi_bus_init(struct pghal_spi *spi);

int pghal_spi_a16_write(struct pghal_spi * spi, uint8_t ss_id, uint16_t address, size_t len, uint8_t * ptr);
int pghal_spi_a16_read(struct pghal_spi * spi, uint8_t ss_id, uint16_t address, size_t len, uint8_t * ptr);
int pghal_spi_write_read(struct pghal_spi * spi, uint8_t ss_id, struct pghal_op_rw * op);


#endif
