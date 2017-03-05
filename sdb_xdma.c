
#include "pghal_inc.h"

#include "sdb_bus.h"
#include "sdb_xdma.h"
#include "sdb_xdma_priv.h"

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

// private structures required only by xdma functions
// no need to export it in headers



struct sdb_node_address * sdb_address_create(struct pghal_bus * bus, uint32_t address)
{
  struct sdb_node_address * ret_addr = (struct sdb_node_address *) pghal_alloc(sizeof(struct sdb_node_address));
  ret_addr->sdb_address = address;
  INIT_LIST_HEAD(&ret_addr->address.list);
  ret_addr->address.bus_driver = bus->driver;
  return ret_addr;
}





// @TODO: implement lock
static void xdma_remap(struct xdma_node * xdma, uint32_t offset)
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

struct pghal_transaction * xdma_op_start(struct pghal_bus * bus, struct pghal_address * address ) {
     struct pghal_transaction * transaction = (struct pghal_transaction *) pghal_alloc(sizeof(struct pghal_transaction));

     transaction->bus = bus;
     transaction->address = address;

     return transaction;
}

static size_t xdma_op_write(struct pghal_bus * bus, struct pghal_address * addr, size_t wr_len, void *wr_ptr )
{
//void * xdma_op_write(struct pghal_bus * bus, struct pghal_transaction * transaction, size_t wr_len, void *wr_ptr ) {
//  struct pghal_bus * hal_bus = transaction->bus; // todo check if bus = bus
  struct pghal_bus * hal_bus = bus;
  struct xdma_node * xdma = (struct xdma_node *) (((void *) hal_bus) - offsetof(struct xdma_node, bus));

//  struct pghal_address * sdb_address = transaction->address;
  struct pghal_address * sdb_address = addr;
  struct sdb_node_address * sdb = (struct sdb_node_address *) (((void *) sdb_address) - offsetof(struct sdb_node_address, address));

  uint32_t address = sdb->sdb_address;
  int i;

  uint32_t * wr_ptr32 = wr_ptr;
  uint32_t * end_ptr = wr_ptr + wr_len;
  while(wr_ptr32 < end_ptr){
    xdma_write_direct(xdma, address, *wr_ptr32);
    wr_ptr32++;
  }
  return wr_len;
}

/*
void pghal_transaction * xdma_op_stop(struct pghal_bus * bus, struct pghal_transaction * transaction ) {
  // TODO: free memory or not
}
*/

static size_t xdma_op_read(struct pghal_bus * bus, struct pghal_address * addr, size_t rd_len, void *rd_ptr )
{
//void * xdma_op_write(struct pghal_bus * bus, struct pghal_transaction * transaction, size_t wr_len, void *wr_ptr ) {
//  struct pghal_bus * hal_bus = transaction->bus; // todo check if bus = bus
  struct pghal_bus * hal_bus = bus;
  struct xdma_node * xdma = (struct xdma_node *) (((void *) hal_bus) - offsetof(struct xdma_node, bus));

//  struct pghal_address * sdb_address = transaction->address;
  struct pghal_address * sdb_address = addr;
  struct sdb_node_address * sdb = (struct sdb_node_address *) (((void *) sdb_address) - offsetof(struct sdb_node_address, address));

  uint32_t address = sdb->sdb_address;
  int i;

  uint32_t * rd_ptr32 = rd_ptr;
  uint32_t * end_ptr = rd_ptr + rd_len;
  while(rd_ptr32 < end_ptr){
    *rd_ptr32 = xdma_read_direct(xdma, address);
//    xdma_write_direct(xdma, address, *wr_ptr32);
    rd_ptr32++;
  }
  return rd_len;
}


uint32_t xdma_read_direct(struct xdma_node * xdma, uint32_t addr) {
  uint32_t read_result = 0 ;

  xdma_remap(xdma, addr);
  void *virt_addr;

  virt_addr = xdma->data + ( addr - xdma->page);
  read_result = *(uint32_t *) virt_addr;
//  printf("xdma_read32 : 0x%08X <- 0x%08X\n", addr, read_result);
  return read_result;
}

void xdma_write_direct(struct xdma_node * xdma, uint32_t addr, uint32_t value) {
  
  xdma_remap(xdma, addr);
  uint32_t read_result = 0 ;
  void *virt_addr;
  
  virt_addr = xdma->data + (addr - xdma->page);
  //printf("xdma_write32: 0x%08X <- 0x%08X\n", addr, value);

  *((uint32_t *) virt_addr) = value;
}

/*
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

*/

static struct xdma_node * xdma_alloc()
{
  struct xdma_node * xdma = NULL;

  xdma = (struct xdma_node *) pghal_alloc(sizeof(struct xdma_node));
  
  struct pghal_bus * bus = & xdma->bus;
  //bus->read  = xdma_read32;
  //bus->write = xdma_write32;
  
  //INIT_LIST_HEAD(&bus->module_list);
  //bus->list.prev = bus->list.next = &bus->list;

  xdma->page_size = MAP_SIZE;
  xdma->page_mask = MAP_MASK;

//  bus_sdb->op_open = xdma_open;
//  bus_sdb->op_close = xdma_close;


  // TODO: add -> op_open, op_close, op_read32, op_write32
  return xdma;
}



static struct pghal_node_driver xdma_node_driver = {
   .node_list = LIST_HEAD_INIT(xdma_node_driver.node_list),
   .list =  LIST_HEAD_INIT(xdma_node_driver.list),
   .probe = NULL
};

static struct pghal_bus_driver xdma_bus_driver = {
  .op = { .start = NULL, .stop = NULL, .write_read = NULL, .write = xdma_op_write , .read = xdma_op_read, .lock = NULL, .unlock = NULL},
  .address = { .snprintf = NULL, .sscanf = NULL, .addr_list = LIST_HEAD_INIT(xdma_bus_driver.address.addr_list) },
  .list = LIST_HEAD_INIT(xdma_bus_driver.list),
  .pghal_node = LIST_HEAD_INIT(xdma_bus_driver.pghal_node)
};

// short path to create address and open it
struct xdma_node * xdma_open_bus(char * address)
{
  struct xdma_node * xdma = NULL; 

  xdma = xdma_alloc();
  if (xdma == NULL) { } // TODO: check if valid alocated

  size_t str_size = strlen(address);

  xdma->address.dev = strndup(address, str_size); // TODO: check if not NULL
  xdma->bus.driver = &xdma_bus_driver;
//  pghal_register_node// 
 
  xdma->fd = open(xdma->address.dev, O_RDWR | O_SYNC);
  xdma_remap(xdma, 0);

  return xdma;
}

void xdma_node_driver_register( struct pghal_list *list)
{
  pghal_node_driver_register(list, &xdma_node_driver);
}


void xdma_bus_driver_register( struct pghal_list *list)
{
  pghal_bus_driver_register(list, &xdma_bus_driver);
}



