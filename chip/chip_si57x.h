
#ifndef __CHIP_SI57X_H__
#define __CHIP_SI57X_H__

/*
 * 571A JC00 0337 DG
 * Product	Si571
 * Description	VCXO; Diff/SE; I2C Prog; 10-1417 MHz
 * Startup Frequency	155.52 MHz
 * I2C Address (Hex Format)	49
 * Format	LVPECL
 * Supply Voltage	3.3 V
 * OE Polarity	OE active high
 * Stability	20 ppm
 * Tuning Slope	135 ppm/V
 * Minimum APR	+/- 130 ppm
 * Frequency Range	10 - 280 MHz
 * Operating Temp Range (°C)	-40 to +85
 */

//SEGMENT_VARIABLE(FDCO_MAX, float, SEG_CODE) = 5670;  //MHz
//SEGMENT_VARIABLE(FDCO_MIN, float, SEG_CODE) = 4850;  //MHz

struct chip_si57x_regs {
   uint8_t  regs_raw[6];

   uint8_t  hsdiv;
   uint8_t  n1;
   double rfreq;
};

struct chip_si57x {
  struct abs_i2c * i2c;
  uint8_t i2c_address;

  char part_number[16];
  
  double fout0; // initial frequency
  double fxtal; // cristal frequency
  struct chip_si57x_regs reg_init;
  struct chip_si57x_regs reg_current;

  double fout_current;
};


struct chip_si57x * chip_si57x_init(struct abs_i2c * i2c_bus);
void chip_si57x_regs_to_val(struct chip_si57x_regs *reg);
void chip_si57x_val_to_regs(struct chip_si57x_regs *reg);

int  chip_si57x_set_from_part_number(struct chip_si57x *chip, char*part_number);
void chip_si57x_set_fout0(struct chip_si57x *chip, float freq);
void chip_si57x_set_address(struct chip_si57x *chip, uint8_t i2c_address);
int  chip_si57x_present(struct chip_si57x *chip);

void chip_si57x_reload_initial(struct chip_si57x *chip);

void chip_si57x_read_fxtal(struct chip_si57x *chip);


#endif
