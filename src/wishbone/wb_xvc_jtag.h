#ifndef __WB_XVC_JTAG_H__
#define __WB_XVC_JTAG_H__

#ifdef __cplusplus
extern "C" {
#endif

// TODO: GPIO abstraction layer required
//

struct  __attribute__((__packed__))  wb_xvc_jtag_reg {
  uint32_t cnt;
  uint32_t tms;
  uint32_t tdi;
  uint32_t tdo;
  uint32_t ctl;
};

struct wb_xvc_jtag {
  struct sdb_module sdb;

  struct wb_xvc_jtag_reg reg;  
};


struct wb_xvc_jtag * wb_xvc_jtag_create_direct(struct pghal_bus * bus, uint32_t bus_address);
void   wb_xvc_jtag_registers_download(struct wb_xvc_jtag * dna);
void   wb_xvc_jtag_registers_upload(struct wb_xvc_jtag * dna, uint32_t cnt, uint32_t tms, uint32_t tdi);
void   wb_xvc_jtag_start(struct wb_xvc_jtag * dna);

#ifdef __cplusplus
}
#endif

#endif
