#ifndef __FMC_ADC250M_H__
#define __FMC_ADC250M_H__

#ifdef __cplusplus
extern "C" {
#endif


struct fmc_adc250m {
   struct pghal_bus * bus;
   uint32_t bus_address;

   struct wb_sdb_rom * sdb;
   uint32_t address[9];

   struct wb_fmc_csr  * csr;   
   struct wb_i2c * i2c_si57x;
   struct wb_spi * spi_ad9510;
   struct wb_spi * spi_isla216p;
   struct wb_gpio_raw * gpio;

   struct chip_si57x    * chip_si57x;
   struct chip_ad9510   * chip_ad9510;
   struct chip_isla216p * chip_isla216p[4];
};

struct fmc_adc250m * fmc_adc250m_init(struct pghal_bus * bus, uint32_t bus_address, uint32_t sdb_address);


#ifdef __cplusplus
}
#endif

#endif

