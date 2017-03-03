
#ifndef __CHIP_AD9510_H__
#define __CHIP_AD9510_H__


struct chip_ad9510 {
  struct pghal_spi * spi;

  uint8_t spi_address; // chip select id

};


struct chip_ad9510 * chip_ad9510_init(struct pghal_spi * spi_bus);
void chip_ad9510_set_address(struct chip_ad9510 *chip, int8_t spi_address);

#endif
