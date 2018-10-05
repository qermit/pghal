/******************************************************************************
 * Title      : SI57x example configuration
 * Project    : Pretty Good Hardware Abstraction Layer
 ******************************************************************************
 * File       : fmc2_config.c
 * Author     : Piotr Miedzik
 * Company    : GSI
 * Created    : 2017-03-01
 * Last update: 2017-03-02
 * Platform   : FPGA-generics
 * Standard   : C
 ******************************************************************************
 * Description:
 * 
 ******************************************************************************
 * Copyleft (ↄ) 2017 Piotr Miedzik
 *****************************************************************************/

#include "pghal_inc.h"

#include "sdb_bus.h"
#include "sdb_uart.h"
#include "sdb_xdma.h"
#include "sdb_xdma_priv.h"


#include "pghal_spi.h"
#include "wishbone/wb_spi.h"

#include "pghal_i2c.h"
#include "wishbone/wb_i2c.h"

#include "wishbone/wb_gpio_raw.h"

#include "wishbone/wb_sdb_rom.h"


// CHIPS include
#include "chip/chip_ad9510.h"
#include "chip/chip_si57x.h"
#include "chip/chip_isla216p.h"


#include "fmc/fmc_adc250m.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

//#include "static_offsets.h"

static struct pghal_list node_drivers_head = LIST_HEAD_INIT(node_drivers_head);
static struct pghal_list bus_drivers_head = LIST_HEAD_INIT(bus_drivers_head);


void print_summary(struct chip_si57x * chip, struct chip_si57x_regs * regs) 
{
  printf("===============================\n");
  printf("Chip si57x summary:\nfout: %lf\nfxtal: %lf MHz\n", regs->fout, chip->fxtal);
  printf("HS_DIV: %d\nN1: %d\n", regs->hsdiv, regs->n1);
  printf("RFREQ: %lf\n", regs->rfreq);
  printf("FDCO: %lf\n", regs->fout * (regs->hsdiv * regs->n1));
  pghal_dump_regs(regs->regs_raw, 6);
  printf("===============================\n");
}

void print_help(char * argv0)
{
  fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n", argv0); 
}


int init_afc(struct fmc_adc250m * fmc_card, double fnew) {

  // set trigger as input
  wb_gpio_raw_set_port_direction(fmc_card->gpio, 0x2000 | 0x20);
  wb_gpio_raw_set_port_value(fmc_card->gpio, 0x2000 | 0x20, 0x2000 | 0x20); 

  wb_gpio_raw_set_port_termination(fmc_card->gpio, 0x8); // <- trigger input termination enable
  wb_gpio_raw_set_port_altf(fmc_card->gpio, 0x0);        // <- reset all alternative functions
  wb_gpio_raw_set_port_value(fmc_card->gpio, 0x20, 0x20 | 0x2000); 

//  wb_gpio_raw_set_port_value(fmc_card->gpio, 0x0400, 0x0400);
  wb_gpio_raw_set_port_value(fmc_card->gpio, 0x0000, 0x0400);
//  usleep(1000);
//  */
  // debug spi and i2c over FMC1 dio;
  // it's bitstream specific
//  wb_gpio_raw_set_port_altf(fmc1_gpio, 0b00000110); 
//  wb_gpio_raw_set_port_direction(fmc1_gpio, 0b00000110); 



  if (1) { 
  if (0) {
    chip_si57x_decode_part_number(fmc_card->chip_si57x, "TESTTESTTEST01");
  } else {
    chip_si57x_decode_part_number(fmc_card->chip_si57x, "571AJC000337DG");
  }  

  chip_si57x_reload_initial(fmc_card->chip_si57x);
  //return 0;
  chip_si57x_find_valid_combo(fmc_card->chip_si57x, fnew, SI57X_METHOD_HARD);
  chip_si57x_send_regs(fmc_card->chip_si57x, &fmc_card->chip_si57x->reg_new, SI57X_METHOD_HARD);
  chip_si57x_registers_download(fmc_card->chip_si57x);

//  return 0;
  // enbable VCXO and PLL
    wb_gpio_raw_set_port_value(fmc_card->gpio, 0x00, 0x20); 
  //  usleep(1);
    wb_gpio_raw_set_port_value(fmc_card->gpio, 0x2000 | 0x20, 0x2000 | 0x20); 
  }
  //return 0;

  if (1) {
    printf("configure AD9510\n");
  // configure AD9510

    chip_ad9510_set_address(fmc_card->chip_ad9510, 0);
    chip_ad9510_soft_reset(fmc_card->chip_ad9510);


//  return 0;
    chip_ad9510_registers_download(fmc_card->chip_ad9510, 0, 100);
    chip_ad9510_config1(fmc_card->chip_ad9510);
    chip_ad9510_registers_download(fmc_card->chip_ad9510, 0, 100); // donload to assert;
    chip_ad9510_registers_commit(fmc_card->chip_ad9510);
  
  }

  //configure 4x ADC
  if (1) {
  // assert and deassert ADC resets
  wb_gpio_raw_set_port_value(fmc_card->gpio, 0x0000, 0x0400);
  usleep(10000);
  wb_gpio_raw_set_port_value(fmc_card->gpio, 0x0400, 0x0400);
  usleep(10000);
  

  int isla_chips = 4;
  // reset first, wait for all in separate loop
  { int i;
    
    for (i = 0 ; i<isla_chips; i++){
      chip_isla216p_set_address(fmc_card->chip_isla216p[i], i);
      chip_isla216p_soft_reset(fmc_card->chip_isla216p[i]);
      fmc_card->chip_isla216p[i]->reg_current.cal[0] = 0;

    }
    // get standard cal time
    usleep(chip_isla216p_get_cal_time_ms(fnew) * 1000);
    for (i = 0 ; i<isla_chips; i++){
      int counter = 0;
      //fmc_card_isla216p[i]->reg_current.cal[0] = 
      chip_isla216p_registers_download(fmc_card->chip_isla216p[i], CHIP_ISLA216P_CAL_ID);
      while( fmc_card->chip_isla216p[i]->reg_current.cal[0] != 0x91) {
        printf("Chip[%d] wait for cal bit: %d -> %02X\n", i, counter++,fmc_card->chip_isla216p[i]->reg_current.cal[0]);
        usleep(1000);
        chip_isla216p_registers_download(fmc_card->chip_isla216p[i], CHIP_ISLA216P_CAL_ID);
      }
      printf("Chip[%d] done after: %d loops\n", i, counter);
      chip_isla216p_registers_download(fmc_card->chip_isla216p[i], CHIP_ISLA216P_ALL_ID);
      printf("Chip[%d] Mode: %02X\n", i, fmc_card->chip_isla216p[i]->reg_current.adc[5]);
      fmc_card->chip_isla216p[i]->reg_current.adc[5] = 0x01;

      fmc_card->chip_isla216p[i]->reg_current.out[2] = 0x01;
      fmc_card->chip_isla216p[i]->reg_current.out[3] = 0x24;
      if (fnew < 90) {
        fmc_card->chip_isla216p[i]->reg_current.out[4] |= 0x40;
      } else {
        fmc_card->chip_isla216p[i]->reg_current.out[4] &= 0xBF;
      }
      chip_isla216p_registers_upload(fmc_card->chip_isla216p[i], CHIP_ISLA216P_OUT_ID | CHIP_ISLA216P_ADC_ID, NULL );
      chip_isla216p_test_path(fmc_card->chip_isla216p[i], ISLA216P_TEST_PATTERN_OFF, 0x0000, 0x0000, 0x0000, 0x0000);
      
      
    }
    
  }
  
  
   }

 return 0;
   
}

int main( int argc, char** argv){
  
  xdma_bus_driver_register(&bus_drivers_head);
  xdma_node_driver_register(&node_drivers_head);
  
  struct pghal_bus * bus = NULL;
  char * devicename = NULL;
  int fmc_id = 1;
  double fnew = 125.0;
  double fnew_tmp = 100.0;

  int c;
  int digit_optind = 0;
  while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"device",  required_argument, 0,  0 },
            {"fmc_id",  required_argument, 0,  0 },
            {"freq",    required_argument, 0,  0 },
            {0,         0,                 0,  0 }
        };
        c = getopt_long(argc, argv, "d:i:f:", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
          case 'd':
            if (devicename != NULL) free(devicename);
            devicename = strdup(optarg);
            break;  
          case 'i':
            if (optarg != NULL)  fmc_id = strtol(optarg, NULL, 0); 
            break;
          case 'f':
            fnew_tmp = strtod(optarg, NULL);
            // @todo: add some checks
            fnew = fnew_tmp;
            break;
          default: 
            print_help(argv[0]);
            exit(EXIT_FAILURE);
        }
   }

  if (fmc_id < 1 || fmc_id > 2) fmc_id = 1;

  if(devicename == NULL) {
    devicename = strdup("/dev/ttyUSB0");
  }

  if (strncmp("/dev/ttyUSB", devicename, strlen("/dev/ttyUSB")) == 0) {
    bus = uart_open_bus(devicename);
  } else if (strncmp("/dev/xdma/", devicename, strlen("/dev/xdma/")) == 0) {
    bus = xdma_open_bus(devicename);
  } else {
    fprintf(stderr, "Unknown driver for device \"%s\"\n", devicename);
            exit(EXIT_FAILURE);
  }
  int fmc_components[2] = {0, 0};
  struct wb_sdb_rom * sdb_rom = wb_sdb_rom_create_direct(bus, 0x0, 0x0); 
  // @todo: automagic fmc search
  //
  uint32_t OFFSET_FMC[2] = {0, 0};
  struct fmc_adc250m * fmc_card[2] = {NULL, NULL};
  uint16_t fmc_component_id = 0;
  if (1) {
    int i = 10;
    char s_name[10];
    char tmp_name[32];
    
    sprintf(s_name, "fmc", fmc_id); 
    for (i=3; i<7 && fmc_component_id < 2; i++){
      int len = wb_sdb_get_name_by_id(sdb_rom, i, tmp_name);
      if (len < 0) continue;
      if (strncmp(s_name, tmp_name,strlen(s_name)) != 0) continue;
      fmc_components[fmc_component_id] = i;
      wb_sdb_get_addr_by_id(sdb_rom, i, &OFFSET_FMC[fmc_component_id]);
      fmc_card[fmc_component_id] = fmc_adc250m_init(bus, OFFSET_FMC[fmc_component_id], OFFSET_FMC[fmc_component_id]);
      printf("Device: %s, fmc_id: %d, offset: %08X -> %s\n", devicename, fmc_component_id, OFFSET_FMC[fmc_component_id], tmp_name);
      fmc_component_id++;
    }
  }
  init_afc(fmc_card[0], fnew);
  init_afc(fmc_card[1], fnew);

}
