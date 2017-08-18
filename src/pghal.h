#ifndef __PGHAL_H__
#define __PGHAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef offsetof 
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif


//#define to_i2c_adapter(d) container_of(d, struct i2c_adapter, dev)
//
//
//Forward declarations
struct pghal_bus_op;
struct pghal_address_driver;
struct pghal_address;
struct pghal_bus_driver;
struct pghal_bus;
struct pghal_node_driver;
struct pghal_node;

struct pghal_header{
  ptrdiff_t offset; // if <0 hoilds offset to "parent" structure
  size_t struct_size;
};

struct pghal_op_rw {
  uint8_t start;
  uint8_t stop;
  size_t granularity; // atomic size 

  size_t wr_offset;
  size_t rd_offset;
  void * wr_ptr;
  void * rd_ptr;
  void * rd_ptr_end;
  void * wr_ptr_end;
};


struct pghal_bus {
  struct pghal_list list; // all buses
  struct pghal_bus_driver * driver;

  struct pghal_list module_list;
};


struct pghal_address_driver {
  void (*snprintf)(char * ptr, size_t size, struct pghal_address * address);
  void (*sscanf)(char * ptr, size_t size, struct pghal_address * address);
// @todo -> zamienic na pghal_list;
  struct pghal_list addr_list; // list to all addresses created
};

struct pghal_address {
  struct pghal_list list; // list to all addresses created
  struct pghal_bus_driver * bus_driver; // pointer to driver
};
  

struct pghal_bus_op {
  void   (*start)  (struct pghal_bus *, struct pghal_address *);
  void   (*stop)   (struct pghal_bus *);
  void   (*write_read) (struct pghal_bus *, size_t length, size_t wr_offset, size_t wr_len, void * wr_ptr, size_t rd_offset, size_t rd_len, void * rd_ptr);
  size_t (*write)  (struct pghal_bus * bus, struct pghal_address * addr, size_t wr_len, void *wr_ptr ); // returns number of written bytes
  size_t (*read)   (struct pghal_bus * bus, struct pghal_address * addr, size_t rd_len, void *rd_ptr ); // return number of readed bytes
  void   (*lock)   (struct pghal_bus *);
  void   (*unlock) (struct pghal_bus *);
};

// abstract bus

// structure common for all nodes
// todo: add operations just like probe
struct pghal_node_driver {
  struct pghal_list node_list; // list all nodes
  struct pghal_list list; // list of all drivers
  int (*probe) (int id);
};

struct pghal_node {
  struct pghal_list list; // all nodes on the bus
  struct pghal_node_driver * driver; // pointer to node driver;
  struct pghal_bus * bus; //pointer to current bus
  struct pghal_address * address;// pointer to address
  
};

struct pghal_bus_driver {
  struct pghal_address_driver address;
  struct pghal_bus_op op;
  struct pghal_list list ; // all bus driver list
  struct pghal_list pghal_node;
};

struct pghal_transaction {
  struct pghal_bus * bus ; //pointer to bus
  struct pghal_address * address; // pointer to address abstraction
};


void pghal_node_driver_register( struct pghal_list *list, struct pghal_node_driver * driver);

void pghal_bus_driver_register( struct pghal_list *list, struct pghal_bus_driver * driver);




size_t pghal_bus_write(struct pghal_bus * bus, struct pghal_address * addr, size_t wr_len, void *wr_ptr );
size_t pghal_bus_read(struct pghal_bus * bus, struct pghal_address * addr, size_t rd_len, void *rd_ptr );

struct sdbbus{
  struct pghal_bus bus;

  char * bus_address;
  
  void      (*op_open) ( struct sdbbus *);
  void     (*op_close) ( struct sdbbus *);
};



/*struct onewire {
  struct sdb_module sdb;
  
};


struct chip_i2c {
  uint8_t i2c_address;
  struct pghal_i2c * bus;
};


struct fmc_dio5 {
  struct sdb_module sdb;

  struct gpio_raw * gpio;
  struct onewire  * onewire;

};

struct fmc_adc250 {
  struct sdb_module sdb;

  
  struct wb_i2c * vcxo_bus;
  struct wb_spi * pll_bus;
  struct wb_spi * adc_bus;
  struct wb_spi * mon_bus;
  struct gpio_raw * gpio_bus;
};
*/


void * pghal_alloc(size_t size); 
void pghal_dump_regs(uint8_t * ptr, size_t len);

/*
struct sdbbus * fmc_sdbbus_alloc(struct sdbbus * handle);
void       fmc_sdbbus_init(struct sdbbus * handle, char * address);
void       fmc_sdbbus_destroy(struct sdbbus * handle);

struct fmc_dio5 * fmc_dio5_alloc(struct fmc_dio5 * handle, struct sdbbus * bus);
void     fmc_dio5_init(struct fmc_dio5 * handle, uint32_t address); 
void     fmc_dio5_destroy(struct fmc_dio5 * handle); 

struct fmc_adc250 * fmc_adc250_alloc(struct fmc_adc250 * handle, struct sdbbus * bus);
void       fmc_adc250_destroy(struct fmc_adc250 * handle); 
*/


#ifdef __cplusplus
}
#endif

#endif
