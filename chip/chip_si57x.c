#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <math.h>

#include "list.h"


#include "pghal.h"
#include "sdb_i2c.h"
#include "chip_si57x.h"


uint8_t VALID_HS_DIV[6] = {11, 9, 7, 6, 5, 4};

struct chip_si57x * chip_si57x_init(struct pghal_i2c * i2c_bus) 
{
  struct chip_si57x * chip = (struct chip_si57x*) pghal_alloc( sizeof(struct chip_si57x));

  // @todo: register to internal bus list???
  
  chip->i2c = i2c_bus;
  chip->config_regs_offset = 0;
  chip->freq_max = -1;
  chip->freq_min = 0;
  chip->fdco_max = 5670.0;
  chip->fdco_min = 4850.0;
  chip_si57x_set_fout0(chip, -1);
 
  return chip;
}


int  chip_si57x_decode_part_number(struct chip_si57x *chip, char*part_number)
{
  if (strncmp("571AJC000337DG", part_number, 12) == 0 ){
    chip_si57x_set_address(chip, 0x49);
    chip_si57x_set_fout0(chip, 155.52);
    chip->config_regs_offset = 7;
    chip->freq_max = 280.0;
    chip->freq_min = 10.0;
    return 0;
  } else {
    return -1;
  }
}

void chip_si57x_set_fout0(struct chip_si57x *chip, double freq) 
{
  chip->reg_init.fout = freq;
}

double chip_si57x_get_fout0(struct chip_si57x *chip) 
{
  return chip->reg_init.fout;
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


int chip_si57x_find_valid_combo(struct chip_si57x * chip, double new_freq, uint8_t use_smooth) 
{
  uint8_t hsdiv;
  uint16_t divider_max = floor(chip->fdco_max / chip->reg_init.fout);
  uint16_t curr_div    = ceil(chip->fdco_min / chip->reg_init.fout);
  uint8_t valid_combo = 0;
  uint8_t i;


  double ratio;
  double curr_n1, n1_tmp;
  uint8_t n1;



  while(curr_div <= divider_max)
  { 
    for(i=0; i<6; i++) 
    {
       hsdiv = VALID_HS_DIV[i];
       curr_n1 = (double)(curr_div) / ((double)(hsdiv));
       n1_tmp = floor(curr_n1);
       n1_tmp = curr_n1 - n1_tmp;
       if (n1_tmp == 0.0) {
         n1 = (uint8_t) curr_n1;
         if((n1 == 1) || (n1 & 1) == 0)
         {
           valid_combo = 1;
         }
       } 
       if(valid_combo == 1) break;
    }
    if (valid_combo = 1) break;

    curr_div++;
  }

  chip->reg_new.fout = new_freq;
  chip->reg_new.hsdiv = hsdiv;
  chip->reg_new.n1 = n1;

  chip->reg_new.rfreq = (chip->reg_new.fout * n1 * hsdiv) / chip->fxtal;
  chip_si57x_val_to_regs(&chip->reg_new); 

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

void chip_si57x_mark_newfreq(struct chip_si57x * chip) 
{
   uint8_t data_w[2] = {135 , 0x20};
  
   pghal_i2c_write_read(chip->i2c, chip->i2c_address, 2, data_w, 0, NULL);
}
void chip_si57x_freeze_dco(struct chip_si57x * chip, uint8_t freeze) 
{
   uint8_t data_w[2] = {136 , 0x08};
   if  (freeze == 0) { data_w[1] = 0 ;}
  
   pghal_i2c_write_read(chip->i2c, chip->i2c_address, 2, data_w, 0, NULL);
}

void chip_si57x_freeze_m(struct chip_si57x * chip, uint8_t freeze) 
{
   uint8_t data_w[2] = {135 , 0x10};
   if  (freeze == 0) { data_w[1] = 0 ;}
  
   pghal_i2c_write_read(chip->i2c, chip->i2c_address, 2, data_w, 0, NULL);
}


void chip_si57x_send_regs(struct chip_si57x * chip, struct chip_si57x_regs * reg, uint8_t method)
{
  // @TODO: check if method suppoted
  // check if pointer is different, then copy it to current
  if (reg != &chip->reg_current) {
     memcpy(&chip->reg_current, reg, sizeof(typeof(chip->reg_current)));
  }
  uint8_t data_w[7];
  data_w[0] = chip->config_regs_offset;
  data_w[1] = chip->reg_current.regs_raw[0];
  data_w[2] = chip->reg_current.regs_raw[1];
  data_w[3] = chip->reg_current.regs_raw[2];
  data_w[4] = chip->reg_current.regs_raw[3];
  data_w[5] = chip->reg_current.regs_raw[4];
  data_w[6] = chip->reg_current.regs_raw[5];
 
  if (method == SI57X_METHOD_HARD) 
    chip_si57x_freeze_dco(chip, 1); 
  else
    chip_si57x_freeze_m(chip, 1); 

  pghal_i2c_write_read(chip->i2c, chip->i2c_address, 7, data_w, 0, NULL);
  
  if (method == SI57X_METHOD_HARD)
  {
    chip_si57x_freeze_dco(chip, 0); 
    chip_si57x_mark_newfreq(chip);
  }
  else
    chip_si57x_freeze_m(chip, 0); 

 // check if new values are written one;

}

void chip_si57x_reload_initial(struct chip_si57x *chip)
{
  struct pghal_i2c * i2c_bus =  chip->i2c;
  chip_si57x_do_recall(chip);
  
  uint8_t * data_w = chip->reg_init.regs_raw;
  data_w[0] = chip->config_regs_offset;
  
  pghal_i2c_write_read(i2c_bus, chip->i2c_address, 1, data_w, 6, data_w);
  int i;
  chip_si57x_regs_to_val(&chip->reg_init);

  chip->fxtal = (chip->reg_init.fout * chip->reg_init.hsdiv * chip->reg_init.n1) / (chip->reg_init.rfreq);

  memcpy(&chip->reg_current, &chip->reg_init, sizeof(typeof(chip->reg_init))); 


}


void chip_si57x_read_fxtal(struct chip_si57x *chip)
{
}


double chip_si57x_get_ppm(double f1, double f2){
  // it's required to divide first;
  return ((f1 / f2) - 1) * 1000000;
}

