
#include "pghal_inc.h"

#include "sdb_bus.h"


void sdb_node_init(struct sdb_module * sdb, struct pghal_bus * bus, uint32_t address) 
{

  sdb->bus = bus; // @TODO: inc bus usage count
  sdb->address.sdb_address = address;
  sdb->address.address.bus_driver = bus->driver;

//  pghal_bus_register_new_module(bus, &handle->list);
}


