
#include "pghal_inc.h"

#include "sdb_bus.h"

struct sdb_node_address * sdb_address_create(struct pghal_bus * bus, uint32_t address)
{
  struct sdb_node_address * ret_addr = (struct sdb_node_address *) pghal_alloc(sizeof(struct sdb_node_address));
  ret_addr->sdb_address = address;
  INIT_LIST_HEAD(&ret_addr->address.list);
  ret_addr->address.bus_driver = bus->driver;
  return ret_addr;
}

void sdb_node_init(struct sdb_module * sdb, struct pghal_bus * bus, uint32_t address) 
{

  sdb->bus = bus; // @TODO: inc bus usage count
  sdb->address.sdb_address = address;
  sdb->address.address.bus_driver = bus->driver;

//  pghal_bus_register_new_module(bus, &handle->list);
}


