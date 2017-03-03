#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "list.h"
#include "pghal.h"
#include "sdb_spi.h"

void pghal_spi_bus_init(struct pghal_spi *spi)
{
  struct pghal_bus * bus = & spi->bus;
  
  INIT_LIST_HEAD(&bus->module_list);
}

void spi_chip_register(struct pghal_spi *spi, uint8_t spi_address)
{
  
}

int pghal_spi_write_read(struct pghal_spi * bus_spi, uint8_t spi_address, size_t wr_size, char * wr_ptr, size_t rd_size, char * rd_ptr) {
  return 0;
}

