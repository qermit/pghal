#ifndef __CHIP_ISLA216P_H__
#define __CHIP_ISLA216P_H__

#ifdef __cplusplus
extern "C" {
#endif

#define CHIP_ISLA216P_REGS_SIZE 256

#define CHIP_ISLA216P_DUT_OFF 0x08
#define CHIP_ISLA216P_DUT_LEN 2
#define CHIP_ISLA216P_ADC_OFF 0x20
#define CHIP_ISLA216P_ADC_LEN 12
#define CHIP_ISLA216P_TEMP_OFF 0x4B
#define CHIP_ISLA216P_TEMP_LEN 3
#define CHIP_ISLA216P_OUT_OFF 0x70
#define CHIP_ISLA216P_OUT_LEN 5
#define CHIP_ISLA216P_CAL_OFF 0xB6
#define CHIP_ISLA216P_CAL_LEN 1
#define CHIP_ISLA216P_TEST_OFF 0xC0
#define CHIP_ISLA216P_TEST_LEN 17


#define CHIP_ISLA216P_DUT_ID  (1 << 0)
#define CHIP_ISLA216P_ADC_ID  (1 << 1)
#define CHIP_ISLA216P_TEMP_ID (1 << 2)
#define CHIP_ISLA216P_OUT_ID  (1 << 3)
#define CHIP_ISLA216P_CAL_ID  (1 << 4)
#define CHIP_ISLA216P_TEST_ID (1 << 5)
#define CHIP_ISLA216P_ALL_ID ((1 << 6) -1)


#define ISLA216P_TEST_PATTERN_OFF 0x00
#define ISLA216P_TEST_PATTERN_MAX 0x20
#define ISLA216P_TEST_PATTERN_MIN 0x30
#define ISLA216P_TEST_PATTERN_RAMP 0xA0
#define ISLA216P_TEST_PATTERN_USER1 0x80
#define ISLA216P_TEST_PATTERN_USER2 0x81
#define ISLA216P_TEST_PATTERN_USER3 0x82
#define ISLA216P_TEST_PATTERN_USER4 0x83


struct chip_isla216p_regs {
   uint8_t dut[2];
   uint8_t adc[12];
   uint8_t temp[3];
   uint8_t out[5];
   uint8_t cal[1];
   uint8_t test[17];
};


struct chip_isla216p {
  struct pghal_spi * spi;
  uint8_t ss_id; // chip select id

  struct chip_isla216p_regs reg_current;
  struct chip_isla216p_regs reg_new;
};

// clock_freq in MHz
uint32_t chip_isla216p_get_cal_time_ms(double clock_freq);

struct chip_isla216p * chip_isla216p_init(struct pghal_spi * spi_bus);
void chip_isla216p_set_address(struct chip_isla216p *chip, int8_t ss_id);


void chip_isla216p_config0(struct chip_isla216p *chip); // test mode
void chip_isla216p_config1(struct chip_isla216p *chip);

void chip_isla216p_test_path(struct chip_isla216p * chip, uint8_t mode, uint16_t b1, uint16_t b2, uint16_t b3, uint16_t b4);
void chip_isla216p_registers_upload(struct chip_isla216p *chip, uint32_t regs, struct chip_isla216p_regs * reg);
void chip_isla216p_registers_download(struct chip_isla216p *chip, uint32_t regs);

#ifdef __cplusplus
}
#endif

#endif
