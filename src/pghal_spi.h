#ifndef __SDB_SPI_H___
#define __SDB_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif


#define PGHAL_SPI_CPHA_BIT (1 << 0)
#define PGHAL_SPI_CPOL_BIT (1 << 1)

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


  uint8_t flags; // tx on rising edge
};


void pghal_spi_bus_init(struct pghal_spi *spi);
void pghal_spi_set_params(struct pghal_spi * spi, uint8_t flags);
int pghal_spi_a16_write(struct pghal_spi * spi, uint8_t ss_id, uint16_t address, size_t len, uint8_t * ptr);
int pghal_spi_a16_read(struct pghal_spi * spi, uint8_t ss_id, uint16_t address, size_t len, uint8_t * ptr);
int pghal_spi_write_read(struct pghal_spi * spi, uint8_t ss_id, struct pghal_op_rw * op);


#ifdef __cplusplus
}
#endif

#endif
