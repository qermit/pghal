
// TODO: GPIO abstraction layer required
struct wb_spi {
  struct sdb_module sdb;
  struct pghal_spi    spi;
};

struct wb_spi * wb_spi_init(struct pghal_bus * bus, uint32_t bus_address);
