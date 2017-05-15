#include "pghal_inc.h"

#include "pghal_spi.h"
#include "chip_ad9510.h"

#define REG_FIRST 0x00
#define REG_LAST  0x5A

#define REG_UPDATE_REGISTERS 0x5A

//struct chip_ad9510 * chip_ad9510_create() ;


void chip_ad9510_registers_commit(struct chip_ad9510 *chip)
{
  uint8_t commit_reg = 1;

  pghal_spi_set_params(chip->spi, 0);
  pghal_spi_a16_write(chip->spi, chip->ss_id, REG_UPDATE_REGISTERS, 1, &commit_reg);
}

void chip_ad9510_registers_upload(struct chip_ad9510 *chip, int start, size_t len)
{
  int i = start;
  if (start < 0) start = 0;
  int end = start+len;
  if (end > REG_LAST) end = REG_LAST;
   

  pghal_spi_set_params(chip->spi, 0);
  
  for (; i <= ( end ); i++) {
    pghal_spi_a16_write(chip->spi, chip->ss_id, i, 1, &chip->regs[i]);
  }
}

void chip_ad9510_registers_download(struct chip_ad9510 *chip, int start, size_t len)
{
  int i = start;
  if (start < 0) start = 0;
  int end = start+len;
  if (end > REG_LAST) end = REG_LAST;
   
  pghal_spi_set_params(chip->spi, 0);
  for (; i <= ( end ); i++) {
    pghal_spi_a16_read(chip->spi, chip->ss_id, i, 1, &chip->regs[i]);
  }
}



void chip_ad9510_soft_reset(struct chip_ad9510 *chip)
{
  
  chip->regs[0] = 0b00111100;
//data_wr[2] = 0b00111100 ; // write 
  //             |||||||+ SDO inactive mirrored
  //             ||||||+- LSB First mirrored
  //             |||||+-- soft reset mirrored (if IC was already initialized with LSB bit already)
  //             ||||+--- long instruction (mirrored)
  //             |||+---- long instruction (only one supported)
  //             ||+----- soft reet
  //             |+------ LSB first
  //             +------- SDO inactive
  pghal_spi_set_params(chip->spi, 0);
  pghal_spi_a16_write(chip->spi, chip->ss_id, 0, 1, &chip->regs[0]);
  usleep(1);
  chip->regs[0] = 0b00011000;
  pghal_spi_a16_write(chip->spi, chip->ss_id, 0, 1, &chip->regs[0]);
}

void chip_ad9510_print_regs(struct chip_ad9510 *chip)
{
  pghal_dump_regs(chip->regs, REG_LAST+1);
}

void chip_ad9510_config0(struct chip_ad9510 *chip)
{
  memcpy(chip->regs_new, chip->regs, sizeof(uint8_t) * 91);
  uint8_t *regs = chip->regs_new;
  // Output[0:3] off
  regs[0x3C] = 0x0A;
  regs[0x3D] = 0x0A;
  regs[0x3E] = 0x0A;
  regs[0x3F] = 0x0A;
  // Output[4:7] off
  // Output[4] on, cmos
  //               +-------------------- Power On
  //               |            +------- CMOS
  regs[0x40] = 0b00000000 | 0b00001000;
  regs[0x41] = 0x03;
  regs[0x42] = 0x03;
  regs[0x43] = 0x03;

  regs[0x45] = 0x1A;

  regs[0x48] = 0x00;
  regs[0x4A] = 0x00;
  regs[0x4C] = 0x00;
  regs[0x4E] = 0x00;

  regs[0x50] = 0x00;
  regs[0x52] = 0x00;
  regs[0x54] = 0x00;
  regs[0x56] = 0x00;

  regs[0x49] = 0x90;
  regs[0x4B] = 0x90;
  regs[0x4D] = 0x90;
  regs[0x4F] = 0x90;

  regs[0x51] = 0x90;
  regs[0x53] = 0x90;
  regs[0x55] = 0x90;
  regs[0x57] = 0x90;

  memcpy(chip->regs, chip->regs_new, sizeof(uint8_t) * 91); 
  chip_ad9510_registers_upload(chip, 0x3C, 8);
  chip_ad9510_registers_upload(chip, 0x45, 1);
  chip_ad9510_registers_upload(chip, 0x48, 16);
}
void chip_ad9510_config1(struct chip_ad9510 *chip)
{
  memcpy(chip->regs_new, chip->regs, sizeof(uint8_t) * 91);
  uint8_t *regs = chip->regs_new;
  // Output[0:3] on
  regs[0x3C] = 0x08;
  regs[0x3D] = 0x08;
  regs[0x3E] = 0x08;
  regs[0x3F] = 0x08;
  // Output[4:7] off
  regs[0x40] = 0b00000000 | 0b00001000;
  regs[0x41] = 0x03;
  regs[0x42] = 0x03;
  regs[0x43] = 0x03;

  regs[0x45] = 0x1A;

  regs[0x48] = 0x00;
  regs[0x4A] = 0x00;
  regs[0x4C] = 0x00;
  regs[0x4E] = 0x00;

  regs[0x49] = 0x90;
  regs[0x4B] = 0x90;
  regs[0x4D] = 0x90;
  regs[0x4F] = 0x90;

  memcpy(chip->regs, chip->regs_new, sizeof(uint8_t) * 91); 
  chip_ad9510_registers_upload(chip, 0x3C, 8);
  chip_ad9510_registers_upload(chip, 0x45, 1);
  chip_ad9510_registers_upload(chip, 0x48, 8);
}

struct chip_ad9510 * chip_ad9510_init(struct pghal_spi * spi_bus ) 
{
  struct chip_ad9510 * chip = (struct chip_ad9510 *) pghal_alloc( sizeof(struct chip_ad9510));

  chip->spi = spi_bus;

  chip_ad9510_set_address(chip, -1);
  
//  chip->spi->write_read
  return chip;
}

void chip_ad9510_set_address(struct chip_ad9510 *chip, int8_t ss_id)
{
  chip->ss_id = ss_id;
}
