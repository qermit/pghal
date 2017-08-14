#ifndef __SDB_XDMA_PRIV_H__
#define __SDB_XDMA_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

struct xdma_node_address {
  struct pghal_address address; 

  char * dev; // pointer to dev
};

struct xdma_node {
  struct pghal_node node; // node information, it should hold one pghal_node_address embedded into xdma_node_address
  struct pghal_bus bus; // bus exported (sdb_bus in case, it holds pointer to druver (and supported OPs)

  struct xdma_node_address address;
    
  // all data required to do write/read ops 
  int fd;
  void * data;
  off_t  page;
  size_t page_size;
  uint32_t page_mask;
};

struct xdma_node * xdma_create_open(char * address);
uint32_t xdma_read_direct(struct xdma_node * xdma, uint32_t addr);
void xdma_write_direct(struct xdma_node * xdma, uint32_t addr, uint32_t value);

#ifdef __cplusplus
}
#endif

#endif
