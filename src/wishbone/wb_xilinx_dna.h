#ifndef __WB_XILINX_DNA_H__
#define __WB_XILINX_DNA_H__

#ifdef __cplusplus
extern "C" {
#endif

// TODO: GPIO abstraction layer required
//

struct  __attribute__((__packed__))  wb_xilinx_dna_reg {
  uint32_t efuseusr;
  uint32_t access2;
  uint64_t dna;
};

struct wb_xilinx_dna {
  struct sdb_module sdb;

  struct wb_xilinx_dna_reg reg;  
};


struct wb_xilinx_dna * wb_xilinx_dna_create_direct(struct pghal_bus * bus, uint32_t bus_address);
void   wb_xilinx_dna_registers_download(struct wb_xilinx_dna * dna);
#ifdef __cplusplus
}
#endif

#endif
