#ifndef __SDB_XDMA_H__
#define __SDB_XDMA_H__

#ifdef __cplusplus
extern "C" {
#endif

struct pghal_bus * xdma_open_bus(char * address);




// TODO: remove later and change to static
struct pghal_transaction * xdma_op_start(struct pghal_bus * bus, struct pghal_address * address );


void xdma_bus_register();

void xdma_bus_driver_register( struct pghal_list *list);


#endif
