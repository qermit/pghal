#include "pghal_inc.h"

#include "sdb_bus.h"
#include "wb_bpm_params.h"

#define WB_FMC_CSR_STATUS 0x00
#define WB_FMC_CSR_CTL1   0x04

#define WB_BPM_PARAM_GAIN0 0x08

//#define WB_GPIO_RAW_DDR  0x08
//#define WB_GPIO_RAW_PSR  0x0C

//#define WB_GPIO_RAW_TERM 0x10
//#define WB_GPIO_RAW_ALTF 0x14

struct wb_bpm_params * wb_bpm_params_create_direct(struct pghal_bus * bus, uint32_t bus_address)
{
  struct wb_bpm_params * csr = NULL;
  csr = (struct wb_bpm_params *) pghal_alloc((sizeof(struct wb_bpm_params)));

  csr->sdb.bus = bus;
  csr->sdb.address.sdb_address =  bus_address;

  return csr;
}

void wb_bpm_params_set_calib(struct wb_bpm_params *csr, int channel, double gain, double offset)
{
  if (channel < 0 || channel > 7) return;
  struct sdb_node_address reg_csr;
  memcpy(&reg_csr, &csr->sdb.address, sizeof(struct sdb_node_address));
 
  gain = gain * 0x8000;
  if (gain > 65535) gain = 0xffff;
  if (gain < 0) gain = 0x0;
  uint16_t gain_fix;
  gain_fix = (uint16_t) gain;

  offset = offset * 0x8000;
  if (offset > 32767) offset = 32767;
  if (offset < -32768) offset =  -32768;
  int16_t  offset_fix;
  offset_fix = (int16_t) offset;

  reg_csr.sdb_address = csr->sdb.address.sdb_address + WB_BPM_PARAM_GAIN0 + (channel * 0x08);
  uint32_t data[2] = { gain_fix, offset_fix} ;

  pghal_bus_write(csr->sdb.bus, &reg_csr.address , 2*sizeof(uint32_t), data);

}

void wb_bpm_params_get_calib(struct wb_bpm_params *csr, int channel, double * gain, double * offset)
{
  if (channel < 0 || channel > 7) return;
  struct sdb_node_address reg_csr;
  memcpy(&reg_csr, &csr->sdb.address, sizeof(struct sdb_node_address));
  reg_csr.sdb_address = csr->sdb.address.sdb_address + WB_BPM_PARAM_GAIN0 + (channel * 0x08);
  uint32_t data[2] = { 0, 0} ;
  uint16_t gain_fix;
  int16_t  offset_fix;

  pghal_bus_read(csr->sdb.bus, &reg_csr.address , 2*sizeof(uint32_t), data);
  gain_fix = (uint16_t) ((data[0]) & 0xFFFF);
  offset_fix = (int16_t) ((data[1]) & 0xFFFF);
 
  *gain   = ((double) gain_fix) / 32768.0; 
  *offset = ((double) offset_fix) / 32768.0; 

}
//void wb_fmc_csr_enable(struct wb_fmc_csr *csr, uint8_t flag)
