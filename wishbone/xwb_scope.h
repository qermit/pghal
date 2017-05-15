#ifndef __XWB_SCOPE_H__
#define __XWB_SCOPE_H__
// TODO: GPIO abstraction layer required


#define XWB_SCOPE_CSR0_RESET           0x01
#define XWB_SCOPE_CSR0_SINGLE_SHOT     0x04

#define XWB_SCOPE_CSR0_STATE_IDLE      0x10
#define XWB_SCOPE_CSR0_STATE_ARMED     0x20
#define XWB_SCOPE_CSR0_STATE_TRIGGERED 0x40

struct __attribute__((__packed__)) xwb_scope_csr {
  uint32_t csr0;
  uint32_t ptr;
  uint32_t start_addr;
  uint32_t end_addr;
};

struct xwb_scope {
  struct sdb_module sdb;

  struct xwb_scope_csr reg;
};


struct xwb_scope * xwb_scope_create_direct(struct pghal_bus * bus, uint32_t bus_address);

void xwb_scope_single_shot(struct xwb_scope * scope);
//void xwb_scope_run(struct xwb_scope * scope);
//void xwb_scope_stop(struct xwb_scope * scope);
void xwb_scope_single_reset(struct xwb_scope * scope);

uint32_t xwb_scope_get_csr(struct xwb_scope * scope);

void xwb_scope_set_address_range(struct xwb_scope * scope, uint32_t start, uint32_t size);
void xwb_scope_registers_download(struct xwb_scope * scope);



static inline int xwb_scope_is_idle(uint32_t csr0) 
{
  if (csr0 & XWB_SCOPE_CSR0_STATE_IDLE) return 1;
  else return 0;
}
static inline int xwb_scope_is_armed(uint32_t csr0) 
{
  if (csr0 & XWB_SCOPE_CSR0_STATE_ARMED) return 1;
  else return 0;
}
static inline int xwb_scope_is_triggered(uint32_t csr0) 
{
  if (csr0 & XWB_SCOPE_CSR0_STATE_TRIGGERED) return 1;
  else return 0;
}



//void     wb_gpio_raw_set_port_direction(struct wb_gpio_raw * gpio, uint32_t dir_out);

/*
uint32_t wb_gpio_raw_get_port_direction(struct * wb_gpio_raw);

*/

//void     wb_gpio_raw_set_port_termination(struct wb_gpio_raw * gpio, uint32_t term_en);

/*
uint32_t wb_gpio_raw_get_port_termination(struct wb_gpio_raw * gpio);
*/
//void     wb_gpio_raw_set_port_altf(struct wb_gpio_raw * gpio, uint32_t altf);
/*
uint32_t wb_gpio_raw_get_port_altf(struct * wb_gpio_raw);

uint32_t wb_gpio_raw_get_port_input(struct * wb_gpio_raw); // returns values from input
uint32_t wb_gpio_raw_get_port_output(struct * wb_gpio_raw); // returns values set to output register 
*/

//void     wb_gpio_raw_set_port_value(struct wb_gpio_raw * gpio, uint32_t value, uint32_t mask); // sets and resets port value

/*
void     wb_gpio_raw_set_port_output(struct * wb_gpio_raw, uint32_t term); // sets port value
void     wb_gpio_raw_reset_port_output(struct * wb_gpio_raw, uint32_t term); // only reset bit
*/

#endif
