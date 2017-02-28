#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "sdbfactory.h"

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

libsdb_header * libsdb_alloc(libsdb_header * handle, size_t size) 
{
   libsdb_header * header = NULL;
   header = (libsdb_header *) malloc(size);
   printf ("libsdb_alloc: %p\n", header);
   memset(header, 0, size);
   return header;
}



struct sdbbus * fmc_sdbbus_alloc(struct sdbbus * handle)
{
}


void       fmc_sdbbus_init(struct sdbbus * handle, char * address)
{
}


void       fmc_sdbbus_destroy(struct sdbbus * handle)
{
}

// @TODO: implement lock
static void xdma_remap(bus_xdma_t * xdma, uint32_t offset)
{
  if (xdma->data != NULL) {
  if (offset >= xdma->page && (offset < (xdma->page + MAP_SIZE))) 
  {
    printf("data inside current mapped page\n");
    return;
  }
    printf("data outside current mapped page - unmapping\n");
    munmap(xdma->data, MAP_SIZE);
    xdma->data = NULL;
  }
  
  xdma->page = offset & (~ MAP_MASK);
  printf("new page: 0x%08X\n" , xdma->page);
  xdma->data = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, xdma->fd, xdma->page & ~MAP_MASK);
  
}


static void xdma_close(struct sdbbus * bus) {
  bus_xdma_t * xdma = (bus_xdma_t *) bus;
  if (xdma->data != NULL) {
    munmap(xdma->data, MAP_SIZE);
    xdma->data = NULL;
  }

  close(xdma->fd);
  xdma->fd = -1;   
}

static void xdma_open(struct sdbbus * bus) {
  bus_xdma_t * xdma = (bus_xdma_t *) bus;
}

static uint32_t xdma_read32(struct sdbbus * bus, uint32_t addr) {
  bus_xdma_t * xdma = (bus_xdma_t *) bus;

  xdma_remap(xdma, addr);
  uint32_t read_result = 0 ;
  void *virt_addr;

  
  virt_addr = xdma->data + ( addr - xdma->page);
  read_result = *(uint32_t *) virt_addr;
  return read_result;
}

static void xdma_write32(struct sdbbus * bus, uint32_t addr, uint32_t value) {
  bus_xdma_t * xdma = (bus_xdma_t *) bus;
  xdma_remap(xdma, addr);
  uint32_t read_result = 0 ;
  void *virt_addr;

  
  virt_addr = xdma->data + (addr - xdma->page);

  *((uint32_t *) virt_addr) = value;
}

bus_xdma_t * xdma_alloc(bus_xdma_t * bus)
{
  bus_xdma_t * ptr = bus;

  ptr = (bus_xdma_t *) libsdb_alloc((libsdb_header *)bus, sizeof(bus_xdma_t));
  
  struct sdbbus * bus_sdb = (struct sdbbus *) ptr;
  bus_sdb->op_read  = xdma_read32;
  bus_sdb->op_write = xdma_write32;
  bus_sdb->op_open = xdma_open;
  bus_sdb->op_close = xdma_close;


  // TODO: add -> op_open, op_close, op_read32, op_write32
  return ptr;
}

bus_xdma_t * xdma_open_bus(bus_xdma_t * bus, char * address)
{
  bus_xdma_t * xdma = bus;
  struct sdbbus * p_sdbbus = NULL;
  if (bus == NULL) {
    xdma = xdma_alloc(NULL);
    if (xdma == NULL) { } // TODO: check if valid alocated
  }

  p_sdbbus = (struct sdbbus *) xdma;
  size_t str_size = strlen(address);

  p_sdbbus->bus_address = strndup(address, str_size); // TODO: check if not NULL
    
  xdma->fd = open(p_sdbbus->bus_address, O_RDWR | O_SYNC);
  xdma_remap(xdma, 0);

  return xdma;
}


dio5_t * fmc_dio5_alloc(dio5_t * handle, struct sdbbus * bus)
{
}

void     fmc_dio5_init(dio5_t * handle, uint32_t address) 
{
}

void     fmc_dio5_destroy(dio5_t * handle) 
{
}

void wb_module_init(struct wb_module * handle, struct sdbbus * bus, uint32_t wb_address)
{
  handle->bus = bus; // @TODO: inc bus usage count
  handle->wb_address = wb_address;
}

struct gpio_raw * gpio_raw_init(struct gpio_raw * handle, struct sdbbus * bus, uint32_t wb_address)
{
  struct gpio_raw * gpio = handle;
  if (gpio == NULL) {
    gpio = (struct gpio_raw *) libsdb_alloc((libsdb_header *) gpio, sizeof(struct gpio_raw));
    wb_module_init(&gpio->wb, bus, wb_address);
  }
  return gpio;
}

struct wb_spi * wb_spi_init(struct wb_spi * handle, struct sdbbus * bus, uint32_t wb_address)
{
  struct wb_spi * spi = handle;
  if (spi == NULL) {
    spi = (struct wb_spi *) libsdb_alloc((libsdb_header *) spi, sizeof(struct wb_spi));
    wb_module_init(&spi->wb, bus, wb_address);
  }
  return spi;
}

#define WB_I2C_REG_PERR_LOW  0x00
#define WB_I2C_REG_PERR_HIGH 0x04
#define WB_I2C_REG_CTR       0x08
#define WB_I2C_REG_RXR_TXR   0x0C
#define WB_I2C_REG_SR_CR     0x10
#define WB_I2C_REG_IFACE     0x14
#define WB_I2C_REG_TXR_DBG   0x18
#define WB_I2C_REG_CR_DBG    0x1C



uint8_t i2c_addr_r(uint8_t addr_7bit)
{
  return ((addr_7bit << 1) | 1);
}
uint8_t i2c_addr_w(uint8_t addr_7bit)
{
  return ((addr_7bit << 1));
}

struct wb_i2c * wb_i2c_init(struct wb_i2c * handle, struct sdbbus * bus, uint32_t wb_address)
{
  struct wb_i2c * i2c = handle;
  if (i2c == NULL) {
    i2c = (struct wb_i2c *) libsdb_alloc((libsdb_header *) i2c, sizeof(struct wb_i2c));
    wb_module_init(&i2c->wb, bus, wb_address);
  }
  return i2c;
}



int wb_i2c_write_read(struct wb_i2c * handle, uint8_t i2c_address, size_t wr_size, char * wr_ptr, size_t rd_size, char * rd_ptr)
{
 uint32_t SR;
 uint32_t FLAG;
 int i;
 // check if just detect i2c
 if (wr_size == 0 && rd_size == 0 ) {
   handle->wb.bus->op_write(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_RXR_TXR, i2c_addr_w(i2c_address));
   handle->wb.bus->op_write(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR, 0b10010000);
   SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
   while (SR & 0x00000002) {
      SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
      if (SR & 0x20 ) return -1;
   }
   handle->wb.bus->op_write(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR,  0b01000000);
   return 0;
 }

  if (wr_size > 0) {
    handle->wb.bus->op_write(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_RXR_TXR, i2c_addr_w(i2c_address));
    handle->wb.bus->op_write(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR, 0b10010000);
    SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
    while (SR & 0x00000002) {
      SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
      if (SR & 0x20 ) return -1;
    }
    for(i=0; i<wr_size; i++) {
      //print("Write to buffer 0x{:0>2X}".format(i2c_op["write_buff"][i]))
      handle->wb.bus->op_write(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_RXR_TXR, wr_ptr[i]);
      handle->wb.bus->op_write(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR, 0b00010000);
      SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
      while (SR & 0x00000002) {
        SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
        if (SR & 0x20 ) return -1;
      }
    }
  }
  if (rd_size > 0) {
    handle->wb.bus->op_write(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_RXR_TXR, i2c_addr_r(i2c_address));
    handle->wb.bus->op_write(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR, 0b10010000);
    SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
    while (SR & 0x00000002) {
      SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
    }

    for(i=0; i<rd_size; i++) {
      SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
      FLAG = 0b00100000;

      if (i == rd_size - 1)
        FLAG = 0b01101000; // # Finish and N ack to finish transfer

      //print("i2c_count_read {} - {} -> flag : {:X} => SR: {:X}".format(i, i2c_op["read_count"],FLAG, SR))

      handle->wb.bus->op_write(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR, FLAG);

      SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
      while (SR & 0x00000002) {
        SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
      }
      rd_ptr[i] = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_RXR_TXR);
    }
  } else {
    printf("finishing witjout read\n");
    handle->wb.bus->op_write(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR, 0b01000000);
  }
  SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
  while (SR & 0x00000040) {
    SR = handle->wb.bus->op_read(handle->wb.bus, handle->wb.wb_address + WB_I2C_REG_SR_CR); 
  }
  return 0;
}

int wb_i2c_detect(struct wb_i2c * handle, uint8_t i2c_address)
{
  return wb_i2c_write_read(handle, i2c_address, 0, NULL, 0, NULL);
}

adc250_t * fmc_adc250_alloc(adc250_t * handle, struct sdbbus * bus)
{
  adc250_t * adc250 = handle;
  if (adc250 == NULL) {
    adc250 = (adc250_t *) libsdb_alloc(NULL,sizeof(adc250_t));
    if (adc250 == NULL) { } // TODO: check if valid alocated
  }
  handle->wb.bus = bus;
  handle->wb.wb_address = 0;
  return adc250;
}

// | +-- 0x00004000 - 0x00005FFF: fmc-adc-250m-16b (sdb@0x00004400)
// | +-- 0x00004000 - 0x0000401F: SPI.AD9510
// | +-- 0x00004020 - 0x0000403F: SPI.ISLA216P
// | +-- 0x00004040 - 0x0000405F: SPI.AMC7823
// | +-- 0x00004100 - 0x000041FF: I2C.Si57x
// | +-- 0x00004200 - 0x000042FF: WB-GPIO-RAW
// | +-- 0x00005000 - 0x00005FFF: IODELAY CTL

adc250_t * fmc_adc250_init(adc250_t * handle, uint32_t address) 
{
   wb_module_init(&handle->wb, handle->wb.bus, address);

   handle->gpio_bus = gpio_raw_init((struct gpio_raw *) NULL, handle->wb.bus, address + 0x200);
   handle->vcxo_bus = wb_i2c_init  ((struct wb_i2c *) NULL,   handle->wb.bus, address + 0x100);

//   handle->vcxo = address + 0x100
//   handlespi_t * pll;
//  spi_t * adc;
//  spi_t * mon;
//  gpio_raw_t * gpio;
//   handle->
}

void       fmc_adc250_destroy(adc250_t * handle) 
{
}

struct chip_si57x  *  si57x_init(struct wb_i2c * i2c_bus, uint8_t i2c_addr) {
}


void fmc_adc250_enable_vcxo(adc250_t * handle, int enable) {
}


int main(){
 bus_xdma_t * xdma = NULL;
 xdma = xdma_open_bus(NULL, "/dev/xdma/card0/user");

 return 0;
 struct sdbbus * bus = (struct sdbbus *)xdma;
 printf("bus_xdma: %p\n", xdma);
 printf("bus_xdma: %p\n", bus->bus_address);
//  asm("int3");
 printf("bus_address: %s\n", bus->bus_address);
// printf("bus_op_read: %p\n", xdma->header.op_read);
 
 printf("BUS_READ 0x200 -> 0x%08X\n", xdma->header.op_read((struct sdbbus *) xdma, 0x200));

 bus->op_write(bus, 0x0, 6);
 
 printf("BUS_READ 0x2200 -> 0x%08X\n", xdma->header.op_read((struct sdbbus *) xdma, 0x2200));

 bus->op_write(bus, 0x4204, 1);
return 0;
}
