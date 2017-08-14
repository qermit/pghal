#ifndef __SDB_UART_H__
#define __SDB_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

struct pghal_bus * uart_open_bus(char * address);




// TODO: remove later and change to static
struct pghal_transaction * uart_op_start(struct pghal_bus * bus, struct pghal_address * address );


void uart_bus_register();

void uart_bus_driver_register( struct pghal_list *list);


#ifdef __cplusplus
}
#endif

#endif
