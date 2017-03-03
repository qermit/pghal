#ifndef __SDB_XDMA_PRIV_H__
#define __SDB_XDMA_PRIV_H__

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

#endif
