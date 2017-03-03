#ifndef __SDB_XDMA_H__
#define __SDB_XDMA_H__

struct xdma_node * xdma_open_bus(char * address);

uint32_t xdma_read_direct(struct xdma_node * xdma, uint32_t addr);
void xdma_write_direct(struct xdma_node * xdma, uint32_t addr, uint32_t value);



// TODO: remove later and change to static
struct pghal_transaction * xdma_op_start(struct pghal_bus * bus, struct pghal_address * address );


void xdma_bus_register();

void xdma_bus_driver_register( struct pghal_list *list);


#endif
