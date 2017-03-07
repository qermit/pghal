#ifndef __SDB_BUS_H__

struct sdb_node_address {
  struct pghal_address address; 

  uint32_t sdb_address;
};

// TODO: opis struktury
struct sdb_entry {
};

struct sdb_module {
  struct sdb_entry entry;
  struct sdb_node_address address;

  struct pghal_bus * bus;

  struct pghal_list list;
};



struct sdb_node_address * sdb_address_create(struct pghal_bus * bus, uint32_t address);

void sdb_node_init(struct sdb_module * sdb, struct pghal_bus * bus, uint32_t address);



#endif
