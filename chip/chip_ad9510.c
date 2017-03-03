#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <math.h>

#include "list.h"


#include "pghal.h"
#include "sdb_spi.h"
#include "chip_ad9510.h"



struct chip_ad9510 * chip_ad9510_init(struct pghal_spi * spi_bus) 
{
  struct chip_ad9510 * chip = (struct chip_ad9510 *) pghal_alloc( sizeof(struct chip_ad9510));

  chip->spi = spi_bus;

  chip_ad9510_set_address(chip, -1);

  return chip;
}



void chip_ad9510_set_address(struct chip_ad9510 *chip, int8_t spi_address)
{
  chip->spi_address = spi_address;
}


