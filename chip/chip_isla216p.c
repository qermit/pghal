#include "pghal_inc.h"

#include "pghal_spi.h"
#include "chip_isla216p.h"


#define CHIP_ISLA216P_MAGIC_CAL_CONSTANT ( 57500.0 * 1.02)
uint32_t chip_isla216p_get_cal_time_ms(double clock_freq)
{
  // clock_freq in MHZ
  double time_in_ms = CHIP_ISLA216P_MAGIC_CAL_CONSTANT / clock_freq;
  return (uint32_t) time_in_ms;
}

void chip_isla216p_registers_upload(struct chip_isla216p *chip, uint32_t regs, struct chip_isla216p_regs * reg)
{
  pghal_spi_set_params(chip->spi, 0);
  int i;
  // chip_id
  if (regs & CHIP_ISLA216P_DUT_ID) 
  for(i=0; i<CHIP_ISLA216P_DUT_LEN; i++) {
    pghal_spi_a16_write(chip->spi, chip->ss_id, CHIP_ISLA216P_DUT_OFF+i,  1, &chip->reg_current.dut[i]);
  }
  if (regs & CHIP_ISLA216P_ADC_ID) 
  for(i=0; i<CHIP_ISLA216P_ADC_LEN; i++) {
    pghal_spi_a16_write(chip->spi, chip->ss_id, CHIP_ISLA216P_ADC_OFF+i,  1, &chip->reg_current.adc[i]);
  }
  if (regs & CHIP_ISLA216P_TEMP_ID) 
  for(i=0; i<CHIP_ISLA216P_TEMP_LEN; i++) {
    pghal_spi_a16_write(chip->spi, chip->ss_id, CHIP_ISLA216P_TEMP_OFF+i, 1, &chip->reg_current.temp[i]);
  }
  if (regs & CHIP_ISLA216P_OUT_ID) 
  for(i=0; i<CHIP_ISLA216P_OUT_LEN; i++) {
    pghal_spi_a16_write(chip->spi, chip->ss_id, CHIP_ISLA216P_OUT_OFF+i,  1, &chip->reg_current.out[i]);
  }
  if (regs & CHIP_ISLA216P_CAL_ID) 
  for(i=0; i<CHIP_ISLA216P_CAL_LEN; i++) {
    pghal_spi_a16_write(chip->spi, chip->ss_id, CHIP_ISLA216P_CAL_OFF+i,  1, &chip->reg_current.cal[i]);
  }
  // reg test_io need to be uploaded last -> upload from highest
  if (regs & CHIP_ISLA216P_TEST_ID) {
     printf("Uploading test sequence (mode %02X):  ",chip->reg_current.test[0]);
  for(i=1; i<CHIP_ISLA216P_TEST_LEN; i++) {
    printf(" %02X", chip->reg_current.test[i]);
    pghal_spi_a16_write(chip->spi, chip->ss_id, CHIP_ISLA216P_TEST_OFF+i, 1, &chip->reg_current.test[i]);
  }
  pghal_spi_a16_write(chip->spi, chip->ss_id, CHIP_ISLA216P_TEST_OFF, 1, &chip->reg_current.test[0]);
    printf("\n");
  }
}

void chip_isla216p_test_path(struct chip_isla216p * chip, uint8_t mode, uint16_t b1, uint16_t b2, uint16_t b3, uint16_t b4)
{
    if (mode == ISLA216P_TEST_PATTERN_OFF ||
        mode == ISLA216P_TEST_PATTERN_MAX ||
        mode == ISLA216P_TEST_PATTERN_RAMP ||
        mode == ISLA216P_TEST_PATTERN_MIN) {
      chip->reg_current.test[0] = mode;
     } else if (    
        mode == ISLA216P_TEST_PATTERN_USER1 ||
        mode == ISLA216P_TEST_PATTERN_USER2 ||
        mode == ISLA216P_TEST_PATTERN_USER3 ||
        mode == ISLA216P_TEST_PATTERN_USER4 ) {
      chip->reg_current.test[0] = mode;

      chip->reg_current.test[1] = (0x00FF & b1);
      chip->reg_current.test[2] = (b1 >> 8);

      chip->reg_current.test[5] = (0x00FF & b2);
      chip->reg_current.test[6] = (b2 >> 8);

      chip->reg_current.test[9] = (0x00FF & b3);
      chip->reg_current.test[10] = (b3 >> 8);

      chip->reg_current.test[13] = (0x00FF & b4);
      chip->reg_current.test[14] = (b4 >> 8);
     
    } else {
      return;
    }
    chip_isla216p_registers_upload(chip, CHIP_ISLA216P_TEST_ID, NULL);
}


void chip_isla216p_registers_download(struct chip_isla216p *chip, uint32_t regs)
{
  pghal_spi_set_params(chip->spi, 0);
  int i;
  // chip_id
  if (regs & CHIP_ISLA216P_DUT_ID) 
  for(i=0; i<CHIP_ISLA216P_DUT_LEN; i++) {
    pghal_spi_a16_read(chip->spi, chip->ss_id, CHIP_ISLA216P_DUT_OFF+i,  1, &chip->reg_current.dut[i]);
  }
  if (regs & CHIP_ISLA216P_ADC_ID) 
  for(i=0; i<CHIP_ISLA216P_ADC_LEN; i++) {
    pghal_spi_a16_read(chip->spi, chip->ss_id, CHIP_ISLA216P_ADC_OFF+i,  1, &chip->reg_current.adc[i]);
  }
  if (regs & CHIP_ISLA216P_TEMP_ID) 
  for(i=0; i<CHIP_ISLA216P_TEMP_LEN; i++) {
    pghal_spi_a16_read(chip->spi, chip->ss_id, CHIP_ISLA216P_TEMP_OFF+i, 1, &chip->reg_current.temp[i]);
  }
  if (regs & CHIP_ISLA216P_OUT_ID) 
  for(i=0; i<CHIP_ISLA216P_OUT_LEN; i++) {
    pghal_spi_a16_read(chip->spi, chip->ss_id, CHIP_ISLA216P_OUT_OFF+i,  1, &chip->reg_current.out[i]);
  }
  if (regs & CHIP_ISLA216P_CAL_ID) 
  for(i=0; i<CHIP_ISLA216P_CAL_LEN; i++) {
    pghal_spi_a16_read(chip->spi, chip->ss_id, CHIP_ISLA216P_CAL_OFF+i,  1, &chip->reg_current.cal[i]);
  }
  if (regs & CHIP_ISLA216P_TEST_ID) 
  for(i=0; i<CHIP_ISLA216P_TEST_LEN; i++) {
    pghal_spi_a16_read(chip->spi, chip->ss_id, CHIP_ISLA216P_TEST_OFF+i, 1, &chip->reg_current.test[i]);
  }
}



void chip_isla216p_soft_reset(struct chip_isla216p *chip)
{
  pghal_spi_set_params(chip->spi, 0);
  uint8_t config_reg = 0b10100101;
//data_wr[2] = 0b00111100 ; // write 
  //             |||||||+ SDO active mirrored
  //             ||||||+- LSB First mirrored
  //             |||||+-- soft reset mirrored (if IC was already initialized with LSB bit already)
  //             ||||+--- reserved (mirrored)
  //             |||+---- reserved
  //             ||+----- soft reet
  //             |+------ LSB first
  //             +------- SDO active
  pghal_spi_a16_write(chip->spi, chip->ss_id, 0, 1, &config_reg);
  usleep(1);
  config_reg = 0b10000001;
  pghal_spi_a16_write(chip->spi, chip->ss_id, 0, 1, &config_reg);
}

void chip_isla216p_print_regs(struct chip_isla216p *chip)
{
//pghal_dump_regs(chip->regs_current, REG_LAST+1);
}

void chip_isla216p_config0(struct chip_isla216p *chip)
{
  memcpy(&chip->reg_new, &chip->reg_current, sizeof(struct chip_isla216p_regs));
  //  uint8_t *regs = chip->regs_new;
  // Output[0:3] off
  //regs[0x3C] = 0x0A;
  //regs[0x3D] = 0x0A;
  //regs[0x3E] = 0x0A;
  //regs[0x3F] = 0x0A;
  //chip_isla216p_registers_upload(chip, 0x3C, 4);
}

struct chip_isla216p * chip_isla216p_init(struct pghal_spi * spi_bus ) 
{
  struct chip_isla216p * chip = (struct chip_isla216p *) pghal_alloc( sizeof(struct chip_isla216p));

  chip->spi = spi_bus;

  chip_isla216p_set_address(chip, -1);
  
//  chip->spi->write_read
  return chip;
}

void chip_isla216p_set_address(struct chip_isla216p *chip, int8_t ss_id)
{
  chip->ss_id = ss_id;
}
