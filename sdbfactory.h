#ifndef __SDB_FACTORY
#define __SDB_FACTORY


#ifndef offsetof 
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif


//#define to_i2c_adapter(d) container_of(d, struct i2c_adapter, dev)

typedef struct {
  size_t struct_size;
  
} libsdb_header;

// abstract bus
struct abs_bus {
  int  test;
  void    ( *write ) ( struct abs_bus *, uint32_t, uint32_t);
  uint32_t ( *read ) ( struct abs_bus *, uint32_t);


  struct list_head module_list;
};

struct sdbbus{
  struct abs_bus bus;

  char * bus_address;
  
  void      (*op_open) ( struct sdbbus *);
  void     (*op_close) ( struct sdbbus *);
};



// TODO: opis struktury
struct sdb_entry {
};

struct sdb_module {
  struct sdb_entry entry;
  uint32_t address;
  struct abs_bus * bus;

  struct list_head list;
};

struct gpio_raw {
  struct sdb_module sdb;
  
} ;

struct onewire {
  struct sdb_module sdb;
  
};


struct wb_spi {
  struct sdb_module sdb;
};

struct chip_i2c {
  uint8_t i2c_address;
  struct abs_i2c * bus;
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

void * libsdb_alloc(size_t size); 

struct gpio_raw * gpio_raw_init(struct gpio_raw * handle, struct sdbbus * bus, uint32_t wb_address);

struct sdbbus * fmc_sdbbus_alloc(struct sdbbus * handle);
void       fmc_sdbbus_init(struct sdbbus * handle, char * address);
void       fmc_sdbbus_destroy(struct sdbbus * handle);

struct fmc_dio5 * fmc_dio5_alloc(struct fmc_dio5 * handle, struct sdbbus * bus);
void     fmc_dio5_init(struct fmc_dio5 * handle, uint32_t address); 
void     fmc_dio5_destroy(struct fmc_dio5 * handle); 

struct fmc_adc250 * fmc_adc250_alloc(struct fmc_adc250 * handle, struct sdbbus * bus);
void       fmc_adc250_destroy(struct fmc_adc250 * handle); 

#endif
