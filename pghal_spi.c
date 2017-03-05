#include "pghal_inc.h"

#include "pghal_spi.h"

void pghal_spi_bus_init(struct pghal_spi *spi)
{
  struct pghal_bus * bus = & spi->bus;
  
  INIT_LIST_HEAD(&bus->module_list);
}

void spi_chip_register(struct pghal_spi *spi, uint8_t spi_address)
{
  
}

// abstract call from struct pghal_bus level
static int _pghal_spi_write_read(struct pghal_bus * bus, struct pghal_address * address, struct pghal_op_rw * op)
{
  struct pghal_spi * spi = (struct pghal_spi *) (((void *) bus) - offsetof(struct pghal_spi, bus));
  struct pghal_spi_address  * spi_address = (struct pghal_spi_address *) (((void *) address) - offsetof(struct pghal_spi_address, address));

//  return pghal_spi_write_read(spi, spi_address->ss_id, op);


}

// direct call
int pghal_spi_write_read(struct pghal_spi * spi, uint8_t ss_id, struct pghal_op_rw * op)
{
  return spi->write_read(spi, ss_id, op);
}

