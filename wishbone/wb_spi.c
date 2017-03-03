
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
#include "wb_spi.h"

#define WB_SPI_RXTX_0 0x00
#define WB_SPI_RXTX_1 0x04
#define WB_SPI_RXTX_2 0x08
#define WB_SPI_RXTX_3 0x0C
#define WB_SPI_CTRL   0x10
#define WB_SPI_DIVIDE 0x14
#define WB_SPI_SS     0x18
#define WB_SPI_RES0   0x1C

struct wb_spi * wb_spi_init(struct pghal_bus * bus, uint32_t bus_address)
{
  struct wb_spi * spi = NULL;
  spi = (struct wb_spi *) pghal_alloc((sizeof(struct wb_spi)));

  sdb_module_init(&spi->sdb, bus, bus_address);
  pghal_spi_bus_init(&spi->spi);

  return spi;
}

