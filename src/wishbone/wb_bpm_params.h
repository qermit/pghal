#ifndef __WB_BPM_PARAMS_H__
#define __WB_BPM_PARAMS_H__

#ifdef __cplusplus
extern "C" {
#endif

struct wb_bpm_params {
  struct sdb_module sdb;
};

struct wb_bpm_params * wb_bpm_params_create_direct(struct pghal_bus * bus, uint32_t bus_address);
void wb_bpm_params_set_calib(struct wb_bpm_params *csr, int channel, double gain, double offset);
void wb_bpm_params_get_calib(struct wb_bpm_params *csr, int channel, double * gain, double * offset);

#ifdef __cplusplus
}
#endif

#endif
