#ifndef __WB_SPI_H__
#define __WB_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

// TODO: GPIO abstraction layer required
struct wb_spi {
  struct sdb_module sdb;
  struct pghal_spi    spi;
};

struct wb_spi * wb_spi_init(struct pghal_bus * bus, uint32_t bus_address);
int wb_spi_write_read_direct(struct wb_spi * wb_spi, uint8_t ss_id, struct pghal_op_rw * op);
#ifdef __cplusplus
}
#endif

#endif
