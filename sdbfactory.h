

typedef struct {
  size_t struct_size;
  
} libsdb_header;

struct sdbbus{
  libsdb_header header;

  char * bus_address;
  
  uint32_t  (*op_read) ( struct sdbbus *, uint32_t) ;
  void     (*op_write) ( struct sdbbus *, uint32_t, uint32_t);
  void      (*op_open) ( struct sdbbus *);
  void     (*op_close) ( struct sdbbus *);
};

typedef struct {
  struct sdbbus header;

  char * bus_address;
  
  int fd;

  void * data;
  off_t     page;
  uint32_t offset;
} bus_xdma_t;

struct wb_module {
  libsdb_header header;
  
  struct sdbbus * bus;
  uint32_t   wb_address;

};

struct gpio_raw {
  struct wb_module wb;
  
} ;

struct onewire {
  struct wb_module wb;
  
};

struct wb_i2c {
  struct wb_module wb;
  
};

struct wb_spi {
  struct wb_module wb;
  
};


typedef struct {
  struct wb_module wb;

  struct gpio_raw * gpio;
  struct onewire  * onewire;

} dio5_t;

typedef struct {
  struct wb_module wb;

  
  struct wb_i2c * vcxo_bus;
  struct wb_spi * pll_bus;
  struct wb_spi * adc_bus;
  struct wb_spi * mon_bus;
  struct gpio_raw * gpio_bus;
} adc250_t;


struct wb_i2c_device {
  libsdb_header header;
  struct wb_i2c * i2c_bus;
  uint8_t i2c_address; 
};

struct chip_si57x {
  struct wb_i2c_device i2c;
  //
};

libsdb_header * libsdb_alloc(libsdb_header * handle, size_t size); 

struct gpio_raw * gpio_raw_init(struct gpio_raw * handle, struct sdbbus * bus, uint32_t wb_address);

struct sdbbus * fmc_sdbbus_alloc(struct sdbbus * handle);
void       fmc_sdbbus_init(struct sdbbus * handle, char * address);
void       fmc_sdbbus_destroy(struct sdbbus * handle);

bus_xdma_t * xdma_open_bus(bus_xdma_t * bus, char * address);

dio5_t * fmc_dio5_alloc(dio5_t * handle, struct sdbbus * bus);
void     fmc_dio5_init(dio5_t * handle, uint32_t address); 
void     fmc_dio5_destroy(dio5_t * handle); 

adc250_t * fmc_adc250_alloc(adc250_t * handle, struct sdbbus * bus);
void       fmc_adc250_destroy(adc250_t * handle); 

