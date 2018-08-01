#ifndef __FMC_DIO32TTLA_H__
#define __FMC_DIO32TTLA_H__

#ifdef __cplusplus
extern "C" {
#endif


struct fmc_dio32ttla {
   struct pghal_bus * bus;
   uint32_t bus_address;

   struct wb_sdb_rom * sdb;
   uint32_t address[9];

   struct wb_fmc_csr  * csr;   
   struct wb_gpio_raw * gpio;
   struct wb_gpio_raw * gpio_info;

};

struct fmc_dio32ttla * fmc_dio32ttla_init(struct pghal_bus * bus, uint32_t bus_address, uint32_t sdb_address);


#ifdef __cplusplus
}
#endif

#endif

