#ifndef __WB_GPIO_RAW_H__
#define __WB_GPIO_RAW_H__

#ifdef __cplusplus
extern "C" {
#endif

// TODO: GPIO abstraction layer required
struct wb_gpio_raw {
  struct sdb_module sdb;

  // temporal addres for writes and reads
  struct sdb_node_address tmp_address;
};


struct wb_gpio_raw * wb_gpio_raw_create_direct(struct pghal_bus * bus, uint32_t bus_address);


void     wb_gpio_raw_set_port_direction(struct wb_gpio_raw * gpio, uint32_t dir_out);

/*
uint32_t wb_gpio_raw_get_port_direction(struct * wb_gpio_raw);

*/

void     wb_gpio_raw_set_port_termination(struct wb_gpio_raw * gpio, uint32_t term_en);

/*
uint32_t wb_gpio_raw_get_port_termination(struct wb_gpio_raw * gpio);
*/
void     wb_gpio_raw_set_port_altf(struct wb_gpio_raw * gpio, uint32_t altf);
/*
uint32_t wb_gpio_raw_get_port_altf(struct * wb_gpio_raw);

uint32_t wb_gpio_raw_get_port_input(struct * wb_gpio_raw); // returns values from input
uint32_t wb_gpio_raw_get_port_output(struct * wb_gpio_raw); // returns values set to output register 
*/

void     wb_gpio_raw_set_port_value(struct wb_gpio_raw * gpio, uint32_t value, uint32_t mask); // sets and resets port value

/*
void     wb_gpio_raw_set_port_output(struct * wb_gpio_raw, uint32_t term); // sets port value
void     wb_gpio_raw_reset_port_output(struct * wb_gpio_raw, uint32_t term); // only reset bit
*/
#ifdef __cplusplus
}
#endif

#endif
