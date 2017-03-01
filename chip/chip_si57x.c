#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "list.h"


#include "sdbfactory.h"
#include "sdb_i2c.h"
#include "chip_si57x.h"



struct chip_si57x * chip_si57x_init(struct abs_i2c * i2c_bus) 
{
  struct chip_si57x * chip = (struct chip_si57x*)libsdb_alloc( sizeof(struct chip_si57x));

  // @todo: register to internal bus list???
  
  chip->i2c = i2c_bus;


}


int  chip_si57x_set_from_part_number(struct chip_si57x *chip, char*part_number)
{
  if (strncmp("571AJC000337DG", part_number, 12) == 0 ){
    chip_si57x_set_address(chip, 0x49);
    chip_si57x_set_fout0(chip, 155.52);
    return 0;
  }
  return -1;
}

void chip_si57x_set_fout0(struct chip_si57x *chip, float freq) 
{
  chip->fout0 = freq;
}

void chip_si57x_set_address(struct chip_si57x *chip, uint8_t i2c_address)
{
  chip->i2c_address = i2c_address;
}

int chip_si57x_present(struct chip_si57x *chip)
{
  return chip->i2c->chip_present(chip->i2c, chip->i2c_address);
}

void chip_si57x_do_recall(struct chip_si57x *chip) {
  uint8_t data_w[2];
  data_w[0] = 135;
  data_w[1] = 1;
  chip->i2c->write_read(chip->i2c, chip->i2c_address, 2, data_w , 0, NULL);
  // @todo: tests reset condition
}

void chip_si57x_regs_to_val(struct chip_si57x_regs *reg) {
  // based on AN334, ReadStartUpConfig
  reg->hsdiv = (( reg->regs_raw[0] & 0xE0) >> 5) + 4;
  reg->n1    = (( reg->regs_raw[0] & 0x1F) << 2) + (( reg->regs_raw[1] & 0xC0 ) >> 6);
  uint64_t rfreq = reg->regs_raw[1] & 0x3F;
  rfreq = (rfreq << 8 ) + reg->regs_raw[2];
  rfreq = (rfreq << 8 ) + reg->regs_raw[3];
  rfreq = (rfreq << 8 ) + reg->regs_raw[4];
  rfreq = (rfreq << 8 ) + reg->regs_raw[5];
  reg->rfreq = rfreq;
  reg->rfreq = reg->rfreq / (268435456.0);   // /2^28

  if (reg->n1 == 0)
  {
    reg->n1 = 1;
  }
  else if ( reg->n1 & 1 != 0)
  {
    reg->n1 = reg->n1 + 1; 
  }
 
}


void chip_si57x_val_to_regs(struct chip_si57x_regs *reg) {
  int i;
  // clear regs
  for(i = 0; i<6 ; i++ ){
    reg->regs_raw[0] = 0;
  }
  
  reg->regs_raw[0] = ((reg->hsdiv - 4) << 5);

  uint8_t n1 = reg->n1;
  uint64_t rfreq = (reg->rfreq * 268435456.0);

  if (n1 == 1)
  {
    n1 = 0;
  }
  else if ( (n1 & 1) == 0)
  {
    n1 = n1 - 1; 
  }
  
  reg->regs_raw[0] = reg->regs_raw[0] | (0x1F & (n1 >> 2));

  reg->regs_raw[5] = (rfreq & 0x0FF);
  rfreq = rfreq >> 8;
  reg->regs_raw[4] = (rfreq & 0x0FF);
  rfreq = rfreq >> 8;
  reg->regs_raw[3] = (rfreq & 0x0FF);
  rfreq = rfreq >> 8;
  reg->regs_raw[2] = (rfreq & 0x0FF);
  rfreq = rfreq >> 8;
  reg->regs_raw[1] = (rfreq & 0x0FF);
  reg->regs_raw[1] = reg->regs_raw[1] | (n1 & 3) << 6;
}

void chip_si57x_reload_initial(struct chip_si57x *chip)
{
  struct abs_i2c * i2c_bus =  chip->i2c;
  chip_si57x_do_recall(chip);
  
  uint8_t * data_w = chip->reg_init.regs_raw;
  data_w[0] = 7;
  
  abs_i2c_write_read(i2c_bus, chip->i2c_address, 1, data_w, 6, data_w);
  int i;
  chip_si57x_regs_to_val(&chip->reg_init);

  chip->fxtal = (chip->fout0 * chip->reg_init.hsdiv * chip->reg_init.n1) / (chip->reg_init.rfreq);

  printf("current fxtal: %lf\n", chip->fxtal);
}


void chip_si57x_read_fxtal(struct chip_si57x *chip)
{
  struct abs_i2c * i2c_bus =  chip->i2c;
  int ret = i2c_bus->chip_present(i2c_bus, chip->i2c_address);
 printf("fmc2_si57x: present %d\n", ret);
}


