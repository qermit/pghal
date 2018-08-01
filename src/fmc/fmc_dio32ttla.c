#include "pghal_inc.h"

#include "fmc_dio32ttla.h"
#include "sdb_bus.h"
#include "wishbone/sdb.h"
#include "wishbone/wb_sdb_rom.h"
#include "wishbone/wb_fmc_csr.h"
#include "wishbone/wb_gpio_raw.h"

#define C_SDB       0
#define C_SYS_I2C   1
#define C_CSR       2
#define C_GPIO      3
#define C_GPIO_INFO 4


#define SDB_LEN 5

struct fmc_dio32ttla * fmc_dio32ttla_init(struct pghal_bus * bus, uint32_t bus_address, uint32_t sdb_address)
{
  struct fmc_dio32ttla * fmc = NULL;
  fmc = (struct fmc_dio32ttla *) pghal_alloc((sizeof(struct fmc_dio32ttla)));

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
    fmc->csr = NULL;
  }

  // not used
  len = wb_sdb_get_addr_by_id(fmc->sdb, C_SYS_I2C, &fmc->address[C_SYS_I2C]);

  len = wb_sdb_get_addr_by_id(fmc->sdb, C_GPIO, &fmc->address[C_GPIO]);
  if (len >= 0) 
    fmc->gpio = wb_gpio_raw_create_direct(bus, fmc->address[C_GPIO]);

  len = wb_sdb_get_addr_by_id(fmc->sdb, C_GPIO_INFO, &fmc->address[C_GPIO_INFO]);
  if (len >= 0) 
    fmc->gpio = wb_gpio_raw_create_direct(bus, fmc->address[C_GPIO_INFO]);
  
  return fmc;   
}

