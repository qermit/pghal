#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>



#include "list.h"
#include "pghal.h"
#include "sdb_xdma.h"

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

// @TODO: implement lock
static void xdma_remap(struct bus_xdma * xdma, uint32_t offset)
{
  if (xdma->data != NULL) {
  if (offset >= xdma->page && (offset < (xdma->page + xdma->page_size))) 
  {
    return;
  }
    munmap(xdma->data, xdma->page_size);
    xdma->data = NULL;
  }
  
  xdma->page = offset & (~ xdma->page_mask);
  printf("new page: 0x%08X\n" , xdma->page);
  xdma->data = mmap(0, xdma->page_size, PROT_READ | PROT_WRITE, MAP_SHARED, xdma->fd, xdma->page);
  
}
/*

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
*/

static uint32_t xdma_read32(struct pghal_bus * bus, uint32_t addr) {
  uint32_t read_result = 0 ;
  struct bus_xdma * xdma = (struct bus_xdma *) bus - offsetof(struct bus_xdma, bus);

  xdma_remap(xdma, addr);
  void *virt_addr;

  virt_addr = xdma->data + ( addr - xdma->page);
  read_result = *(uint32_t *) virt_addr;
//  printf("xdma_read32 : 0x%08X <- 0x%08X\n", addr, read_result);
  return read_result;
}

static void xdma_write32(struct pghal_bus * bus, uint32_t addr, uint32_t value) {
  struct bus_xdma * xdma = (struct bus_xdma *) bus - offsetof(struct bus_xdma, bus);
  
  xdma_remap(xdma, addr);
  uint32_t read_result = 0 ;
  void *virt_addr;

  
  virt_addr = xdma->data + (addr - xdma->page);
  //printf("xdma_write32: 0x%08X <- 0x%08X\n", addr, value);

  *((uint32_t *) virt_addr) = value;
}

static struct bus_xdma * xdma_alloc()
{
  struct bus_xdma * xdma = NULL;

  xdma = (struct bus_xdma *) pghal_alloc(sizeof(struct bus_xdma));
  
  struct pghal_bus * bus = & xdma->bus;
  bus->read  = xdma_read32;
  bus->write = xdma_write32;
  
  INIT_LIST_HEAD(&bus->module_list);
  //bus->list.prev = bus->list.next = &bus->list;

  xdma->page_size = MAP_SIZE;
  xdma->page_mask = MAP_MASK;
//  bus_sdb->op_open = xdma_open;
//  bus_sdb->op_close = xdma_close;


  // TODO: add -> op_open, op_close, op_read32, op_write32
  return xdma;
}

struct bus_xdma * xdma_open_bus(char * address)
{
  struct bus_xdma * xdma = NULL; 

  xdma = xdma_alloc();
  if (xdma == NULL) { } // TODO: check if valid alocated

  size_t str_size = strlen(address);

  xdma->bus_address = strndup(address, str_size); // TODO: check if not NULL
    
  xdma->fd = open(xdma->bus_address, O_RDWR | O_SYNC);
  xdma_remap(xdma, 0);

  return xdma;
}

