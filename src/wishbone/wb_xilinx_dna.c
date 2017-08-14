#include "pghal_inc.h"

#include "sdb_bus.h"
#include "wb_xilinx_dna.h"



struct wb_xilinx_dna * wb_xilinx_dna_create_direct(struct pghal_bus * bus, uint32_t bus_address)
{
  struct wb_xilinx_dna * dna = NULL;
  dna = (struct wb_xilinx_dna *) pghal_alloc((sizeof(struct wb_xilinx_dna)));

  dna->sdb.bus = bus;
  dna->sdb.address.sdb_address =  bus_address;

  return dna;
}

void   wb_xilinx_dna_registers_download(struct wb_xilinx_dna * dna)
{
 
  uint32_t regs[4]; 
  struct sdb_node_address reg_dna;
  memcpy(&reg_dna, &dna->sdb.address, sizeof(struct sdb_node_address));

  pghal_bus_read(dna->sdb.bus, &reg_dna.address, sizeof(struct wb_xilinx_dna_reg), &dna->reg);

}

