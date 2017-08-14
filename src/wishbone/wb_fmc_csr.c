#include "pghal_inc.h"

#include "sdb_bus.h"
#include "wb_fmc_csr.h"

#define WB_FMC_CSR_STATUS 0x00
#define WB_FMC_CSR_CTL1   0x04
//#define WB_GPIO_RAW_DDR  0x08
//#define WB_GPIO_RAW_PSR  0x0C

//#define WB_GPIO_RAW_TERM 0x10
//#define WB_GPIO_RAW_ALTF 0x14

struct wb_fmc_csr * wb_fmc_csr_create_direct(struct pghal_bus * bus, uint32_t bus_address)
{
  struct wb_fmc_csr * csr = NULL;
  csr = (struct wb_fmc_csr *) pghal_alloc((sizeof(struct wb_fmc_csr)));

  csr->sdb.bus = bus;
  csr->sdb.address.sdb_address =  bus_address;

  return csr;
}

void wb_fmc_csr_enable(struct wb_fmc_csr *csr, uint8_t flag)
{
  struct sdb_node_address reg_csr;
  memcpy(&reg_csr, &csr->sdb.address, sizeof(struct sdb_node_address));
 
  reg_csr.sdb_address = csr->sdb.address.sdb_address + WB_FMC_CSR_CTL1; 
  uint32_t data[1];

  if (flag) { data[0] = 0x10; } else { data[0] = 0x00; }

  pghal_bus_write(csr->sdb.bus, &reg_csr.address , 1*sizeof(uint32_t), data);

}

