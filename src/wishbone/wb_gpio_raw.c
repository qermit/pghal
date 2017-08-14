#include "pghal_inc.h"

#include "sdb_bus.h"
#include "wb_gpio_raw.h"

#define WB_GPIO_RAW_CODR 0x00
#define WB_GPIO_RAW_SODR 0x04
#define WB_GPIO_RAW_DDR  0x08
#define WB_GPIO_RAW_PSR  0x0C

#define WB_GPIO_RAW_TERM 0x10
#define WB_GPIO_RAW_ALTF 0x14

struct wb_gpio_raw * wb_gpio_raw_create_direct(struct pghal_bus * bus, uint32_t bus_address)
{
  struct wb_gpio_raw * gpio = NULL;
  gpio = (struct wb_gpio_raw *) pghal_alloc((sizeof(struct wb_gpio_raw)));

  gpio->sdb.bus = bus;
  gpio->sdb.address.sdb_address =  bus_address;
  gpio->tmp_address.sdb_address =  bus_address;


  
//  sdb_module_init(&gpio->sdb, bus, bus_address);

  return gpio;
}
void     wb_gpio_raw_set_port_termination(struct wb_gpio_raw * gpio, uint32_t term_en) {
  uint32_t data_w[1] = {term_en};
  struct sdb_node_address reg_term;
  memcpy(&reg_term, &gpio->sdb.address, sizeof(struct sdb_node_address));
  reg_term.sdb_address = reg_term.sdb_address + WB_GPIO_RAW_TERM;
  pghal_bus_write(gpio->sdb.bus, &reg_term.address , 1*sizeof(uint32_t), data_w);
}

void     wb_gpio_raw_set_port_altf(struct wb_gpio_raw *gpio, uint32_t altf) {
  uint32_t data_w[1] = {altf};
  struct sdb_node_address reg_altf;
  memcpy(&reg_altf, &gpio->sdb.address, sizeof(struct sdb_node_address));
  reg_altf.sdb_address = reg_altf.sdb_address + WB_GPIO_RAW_ALTF;
  pghal_bus_write(gpio->sdb.bus, &reg_altf.address , 1*sizeof(uint32_t), data_w);
}

void     wb_gpio_raw_set_port_direction(struct wb_gpio_raw * gpio, uint32_t dir_out){
  uint32_t data_w[1] = {dir_out};
  struct sdb_node_address reg_altf;
  memcpy(&reg_altf, &gpio->sdb.address, sizeof(struct sdb_node_address));
  reg_altf.sdb_address = reg_altf.sdb_address + WB_GPIO_RAW_DDR;
  pghal_bus_write(gpio->sdb.bus, &reg_altf.address , 1*sizeof(uint32_t), data_w);
}

void wb_gpio_raw_set_port_value(struct wb_gpio_raw * gpio, uint32_t value, uint32_t mask)
{
  uint32_t data_w[2] = { value & mask, (~value) & mask };  
  // TODO: memory leak :)
  gpio->tmp_address.sdb_address = gpio->sdb.address.sdb_address + WB_GPIO_RAW_SODR; 
  pghal_bus_write(gpio->sdb.bus, &gpio->tmp_address.address , 1*sizeof(uint32_t), data_w);
  gpio->tmp_address.sdb_address = gpio->sdb.address.sdb_address + WB_GPIO_RAW_CODR; 
  pghal_bus_write(gpio->sdb.bus, &gpio->tmp_address.address, 1*sizeof(uint32_t), data_w+1);
//   pghal_bus_write(gpio->sdb.bus, gpio->sdb.address + WB_GPIO_RAW_SODR, value & mask);
//   pghal_bus_write(gpio->sdb.bus, gpio->sdb.address + WB_GPIO_RAW_CODR, (~value) & mask);
 
}

