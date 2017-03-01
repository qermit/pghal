#ifndef __SDB_XDMA_H__
#define __SDB_XDMA_H__


struct bus_xdma {
  struct abs_bus bus;

  char * bus_address;
  
  int fd;

  void * data;
  off_t  page;
  size_t page_size;
  uint32_t page_mask;
};

struct bus_xdma * xdma_open_bus(char * address);

#endif
