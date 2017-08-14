#ifndef __CHIP_AD9510_H__
#define __CHIP_AD9510_H__

#ifdef __cplusplus
extern "C" {
#endif

struct chip_ad9510 {
  struct pghal_spi * spi;

  uint8_t ss_id; // chip select id


  uint8_t regs[91]; // copy of all registers
  uint8_t regs_new[91]; // copy of all registers
};


struct chip_ad9510 * chip_ad9510_init(struct pghal_spi * spi_bus);
void chip_ad9510_set_address(struct chip_ad9510 *chip, int8_t spi_address);

void chip_ad9510_prepare_write(struct pghal_op_rw * spi_op, int16_t reg_address, size_t wr_len);
void chip_ad9510_prepare_read(struct pghal_op_rw * spi_op, int16_t reg_address, size_t len);

void chip_ad9510_config0(struct chip_ad9510 *chip);
void chip_ad9510_config1(struct chip_ad9510 *chip);

void chip_ad9510_print_regs(struct chip_ad9510 *chip);  
void chip_ad9510_registers_commit(struct chip_ad9510 *chip);
void chip_ad9510_registers_upload(struct chip_ad9510 *chip, int start, size_t len);
void chip_ad9510_registers_download(struct chip_ad9510 *chip, int start, size_t len);

#ifdef __cplusplus
}
#endif

#endif
