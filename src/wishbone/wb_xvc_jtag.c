#include "pghal_inc.h"

#include "sdb_bus.h"
#include "wb_xvc_jtag.h"



struct wb_xvc_jtag * wb_xvc_jtag_create_direct(struct pghal_bus * bus, uint32_t bus_address)
{
  struct wb_xvc_jtag * xvc_jtag = NULL;
  xvc_jtag = (struct wb_xvc_jtag *) pghal_alloc((sizeof(struct wb_xvc_jtag)));

  xvc_jtag->sdb.bus = bus;
  xvc_jtag->sdb.address.sdb_address =  bus_address;

  return xvc_jtag;
}

void  wb_xvc_jtag_registers_download(struct wb_xvc_jtag * dna)
{
  uint32_t regs[2]; 
  struct sdb_node_address reg_dna;
  memcpy(&reg_dna, &dna->sdb.address, sizeof(struct sdb_node_address));
  reg_dna.sdb_address = reg_dna.sdb_address + 0xC;

  pghal_bus_read(dna->sdb.bus, &reg_dna.address, 2*sizeof(uint32_t), regs);
  dna->reg.tdo = regs[0];
  dna->reg.ctl = regs[1];
  if (dna->reg.ctl == 0)
    printf("wb_xvc_jtag_registers_download: %08x, %08x\n", dna->reg.tdo, dna->reg.ctl);
}
void wb_xvc_jtag_registers_upload(struct wb_xvc_jtag * dna, uint32_t cnt, uint32_t tms, uint32_t tdi)
{
  printf("wb_xvc_jtag_registers_upload: %08x, %08x, %08x\n", cnt, tms, tdi);  
  uint32_t data_w[3] = {cnt, tms, tdi};
  struct sdb_node_address reg_altf;
  memcpy(&reg_altf, &dna->sdb.address, sizeof(struct sdb_node_address));

  pghal_bus_write(dna->sdb.bus, &reg_altf.address , 3*sizeof(uint32_t), data_w);
  
}

void wb_xvc_jtag_start(struct wb_xvc_jtag * dna)
{
  uint32_t data_w[1] = {1};
  struct sdb_node_address reg_altf;
  memcpy(&reg_altf, &dna->sdb.address, sizeof(struct sdb_node_address));
  reg_altf.sdb_address = reg_altf.sdb_address + 0x10;
  pghal_bus_write(dna->sdb.bus, &reg_altf.address , 1*sizeof(uint32_t), data_w);
}

