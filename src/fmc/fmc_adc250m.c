#include "pghal_inc.h"

#include "fmc_adc250m.h"
#include "sdb_bus.h"
#include "wishbone/sdb.h"
#include "wishbone/wb_sdb_rom.h"
#include "wishbone/wb_fmc_csr.h"
#include "pghal_i2c.h"
#include "wishbone/wb_i2c.h"
#include "pghal_spi.h"
#include "wishbone/wb_spi.h"
#include "wishbone/wb_gpio_raw.h"

#include "chip/chip_ad9510.h"
#include "chip/chip_si57x.h"
#include "chip/chip_isla216p.h"

#define C_SDB      0
#define C_CSR      1
#define C_SYS_I2C  2
#define C_I2C_VCXO 3
#define C_SPI_PLL  4
#define C_SPI_ADC  5
#define C_SPI_MON  6
#define C_GPIO     7
#define C_IDELAY   8

#define SDB_LEN 9

struct fmc_adc250m * fmc_adc250m_init(struct pghal_bus * bus, uint32_t bus_address, uint32_t sdb_address)
{
  struct fmc_adc250m * fmc = NULL;
  uint16_t fmc_id_shift = 0;
  fmc = (struct fmc_adc250m *) pghal_alloc((sizeof(struct fmc_adc250m)));

  fmc->bus = bus;
  fmc->bus_address = bus_address;
  fmc->sdb = wb_sdb_rom_create_direct(bus, bus_address, sdb_address);

  fmc->address[C_SDB] = sdb_address;

  // todo: correct adc version handling with wb_sdb_get_by_ids
  int len = wb_sdb_get_addr_by_id(fmc->sdb, C_CSR, &fmc->address[C_CSR]);
  if (len >= 0)  {
    fmc->csr = wb_fmc_csr_create_direct(bus, fmc->address[C_CSR]);
    wb_fmc_csr_enable(fmc->csr, 1);  
    
  } else {
    fmc_id_shift = 1;
    fmc->csr = NULL;
  }

  len = wb_sdb_get_addr_by_id(fmc->sdb, C_SYS_I2C - fmc_id_shift, &fmc->address[C_SYS_I2C]);

  len = wb_sdb_get_addr_by_id(fmc->sdb, C_I2C_VCXO - fmc_id_shift, &fmc->address[C_I2C_VCXO]);
  if (len >= 0) {
    fmc->i2c_si57x = wb_i2c_init(bus, fmc->address[C_I2C_VCXO]);
    fmc->chip_si57x = chip_si57x_init(&fmc->i2c_si57x->i2c);
  }
    
  len = wb_sdb_get_addr_by_id(fmc->sdb, C_SPI_PLL - fmc_id_shift, &fmc->address[C_SPI_PLL]);
  if (len >= 0) {
    fmc->spi_ad9510  = wb_spi_init(bus, fmc->address[C_SPI_PLL]);
    fmc->chip_ad9510 = chip_ad9510_init(&fmc->spi_ad9510->spi);
  }

  len = wb_sdb_get_addr_by_id(fmc->sdb, C_SPI_ADC - fmc_id_shift , &fmc->address[C_SPI_ADC]);
  if (len >= 0) {
    int i;
    fmc->spi_isla216p = wb_spi_init(bus, fmc->address[C_SPI_ADC]);
    for(i=0 ; i< 4; i++) {
      fmc->chip_isla216p[i] = chip_isla216p_init(&fmc->spi_isla216p->spi);
    }
  }
  len = wb_sdb_get_addr_by_id(fmc->sdb, C_SPI_MON - fmc_id_shift, &fmc->address[C_SPI_MON]);
  len = wb_sdb_get_addr_by_id(fmc->sdb, C_GPIO - fmc_id_shift, &fmc->address[C_GPIO]);
  if (len >= 0) 
    fmc->gpio = wb_gpio_raw_create_direct(bus, fmc->address[C_GPIO]);

  len = wb_sdb_get_addr_by_id(fmc->sdb, C_IDELAY - fmc_id_shift, &fmc->address[C_IDELAY]);
   
  
  return fmc;   
}

