#ifndef __WB_FMC_CSR_H__
#define __WB_FMC_CSR_H__

#ifdef __cplusplus
extern "C" {
#endif

struct wb_fmc_csr {
  struct sdb_module sdb;
};

struct wb_fmc_csr * wb_fmc_csr_create_direct(struct pghal_bus * bus, uint32_t bus_address);
void wb_fmc_csr_enable(struct wb_fmc_csr *csr, uint8_t flag);

#ifdef __cplusplus
}
#endif

#endif
