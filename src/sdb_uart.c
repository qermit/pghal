#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include "pghal_inc.h"

#include "sdb_bus.h"
#include "sdb_uart.h"
#include "sdb_uart_priv.h"

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

// private structures required only by uart functions
// no need to export it in headers







/*

static void uart_close(struct sdbbus * bus) {
  bus_uart_t * uart = (bus_uart_t *) bus;
  if (uart->data != NULL) {
    munmap(uart->data, MAP_SIZE);
    uart->data = NULL;
  }

  close(uart->fd);
  uart->fd = -1;   
}

static void uart_open(struct sdbbus * bus) {
  bus_uart_t * uart = (bus_uart_t *) bus;
}
*/

struct pghal_transaction * uart_op_start(struct pghal_bus * bus, struct pghal_address * address ) {
     struct pghal_transaction * transaction = (struct pghal_transaction *) pghal_alloc(sizeof(struct pghal_transaction));

     transaction->bus = bus;
     transaction->address = address;

     return transaction;
}

static size_t uart_op_write(struct pghal_bus * bus, struct pghal_address * addr, size_t wr_len, void *wr_ptr )
{

  struct pghal_bus * hal_bus = bus;
  struct uart_node * uart = (struct uart_node *) (((void *) hal_bus) - offsetof(struct uart_node, bus));

  struct pghal_address * sdb_address = addr;
  struct sdb_node_address * sdb = (struct sdb_node_address *) (((void *) sdb_address) - offsetof(struct sdb_node_address, address));

  uint32_t address = sdb->sdb_address;
  int i;

  uint32_t * wr_ptr32 = wr_ptr;
  uint32_t * end_ptr = wr_ptr + wr_len;
  while(wr_ptr32 < end_ptr){
    uart_write_direct(uart, address, *wr_ptr32);
    address += sizeof(uint32_t);
    wr_ptr32++;
  }
  
  return wr_len;
}

/*
void pghal_transaction * uart_op_stop(struct pghal_bus * bus, struct pghal_transaction * transaction ) {
  // TODO: free memory or not
}
*/

static size_t uart_op_read(struct pghal_bus * bus, struct pghal_address * addr, size_t rd_len, void *rd_ptr )
{
//void * uart_op_write(struct pghal_bus * bus, struct pghal_transaction * transaction, size_t wr_len, void *wr_ptr ) {
//  struct pghal_bus * hal_bus = transaction->bus; // todo check if bus = bus
  struct pghal_bus * hal_bus = bus;
  struct uart_node * uart = (struct uart_node *) (((void *) hal_bus) - offsetof(struct uart_node, bus));

//  struct pghal_address * sdb_address = transaction->address;
  struct pghal_address * sdb_address = addr;
  struct sdb_node_address * sdb = (struct sdb_node_address *) (((void *) sdb_address) - offsetof(struct sdb_node_address, address));

  uint32_t address = sdb->sdb_address;
  int i;

  uint32_t * rd_ptr32 = rd_ptr;
  uint32_t * end_ptr = rd_ptr + rd_len;
  
  while(rd_ptr32 < end_ptr){
    *rd_ptr32 = uart_read_direct(uart, address);
    address += sizeof(uint32_t);
    rd_ptr32++;
  }
  return rd_len;
}


uint32_t uart_read_direct(struct uart_node * uart, uint32_t addr) {
  uint32_t read_result = 0 ;
  int res;
  char buffer[20];
  int len = sprintf(buffer, "r%08X\r", addr);

  write(uart->fd,buffer,len);
  res = read(uart->fd, buffer, 11);
//  fprintf(stderr, "<%s\n",buffer);
  char status;
  sscanf(buffer,"%c%08X", &status, &read_result);
  if (uart->debug)
    fprintf(stderr, "<r%08X:%c%08X\n",addr,status,read_result);
  
  return read_result;

}

void uart_write_direct(struct uart_node * uart, uint32_t addr, uint32_t value) {
  uint32_t read_result = 0 ;
  int res;
  char buffer[30];
  int len = sprintf(buffer, "d%08X\rw%08X\r", value, addr);
  if (uart->debug)
    fprintf(stderr, ">w%08X:d%08X\n",addr, value);
  write(uart->fd,buffer,len);

  res = read(uart->fd, buffer, 11);
//  fprintf(stderr, "<%s\n",buffer);
  char status;
  sscanf(buffer,"%c%08X", &status, &read_result);
}

/*
static uint32_t uart_read32(struct pghal_bus * bus, uint32_t addr) {
  uint32_t read_result = 0 ;
  struct bus_uart * uart = (struct bus_uart *) bus - offsetof(struct bus_uart, bus);

  uart_remap(uart, addr);
  void *virt_addr;

  virt_addr = uart->data + ( addr - uart->page);
  read_result = *(uint32_t *) virt_addr;
//  printf("uart_read32 : 0x%08X <- 0x%08X\n", addr, read_result);
  return read_result;
}

static void uart_write32(struct pghal_bus * bus, uint32_t addr, uint32_t value) {
  struct bus_uart * uart = (struct bus_uart *) bus - offsetof(struct bus_uart, bus);
  
  uart_remap(uart, addr);
  uint32_t read_result = 0 ;
  void *virt_addr;

  
  virt_addr = uart->data + (addr - uart->page);
  //printf("uart_write32: 0x%08X <- 0x%08X\n", addr, value);

  *((uint32_t *) virt_addr) = value;
}

*/

static struct uart_node * uart_alloc()
{
  struct uart_node * uart = NULL;

  uart = (struct uart_node *) pghal_alloc(sizeof(struct uart_node));
  
  struct pghal_bus * bus = & uart->bus;
  //bus->read  = uart_read32;
  //bus->write = uart_write32;
  
  //INIT_LIST_HEAD(&bus->module_list);
  //bus->list.prev = bus->list.next = &bus->list;

//  uart->page_size = MAP_SIZE;
//  uart->page_mask = MAP_MASK;

//  bus_sdb->op_open = uart_open;
//  bus_sdb->op_close = uart_close;


  // TODO: add -> op_open, op_close, op_read32, op_write32
  return uart;
}



static struct pghal_node_driver uart_node_driver = {
   .node_list = LIST_HEAD_INIT(uart_node_driver.node_list),
   .list =  LIST_HEAD_INIT(uart_node_driver.list),
   .probe = NULL
};

static struct pghal_bus_driver uart_bus_driver = {
  .op = { .start = NULL, .stop = NULL, .write_read = NULL, .write = uart_op_write , .read = uart_op_read, .lock = NULL, .unlock = NULL},
  .address = { .snprintf = NULL, .sscanf = NULL, .addr_list = LIST_HEAD_INIT(uart_bus_driver.address.addr_list) },
  .list = LIST_HEAD_INIT(uart_bus_driver.list),
  .pghal_node = LIST_HEAD_INIT(uart_bus_driver.pghal_node)
};

// short path to create address and open it
struct uart_node * uart_creat_open(char * address)
{
  struct uart_node * uart = NULL; 

  uart = uart_alloc();
  if (uart == NULL) { } // TODO: check if valid alocated

  size_t str_size = strlen(address);

  uart->address.dev = strndup(address, str_size); // TODO: check if not NULL
  uart->bus.driver = &uart_bus_driver;

  int c, res;
  char buf[255];
  uart->fd = open(address, O_RDWR | O_NOCTTY ); 
  if (uart->fd <0) {perror(address); exit(-1); }
  
  tcgetattr(uart->fd,&uart->oldtio); /* save current port settings */
  
  bzero(&uart->newtio, sizeof(uart->newtio));
  uart->newtio.c_cflag = B115200 | CRTSCTS | CS8 | CLOCAL | CREAD;
  uart->newtio.c_iflag = IGNPAR;
  uart->newtio.c_oflag = 0;
  
  /* set input mode (non-canonical, no echo,...) */
  uart->newtio.c_lflag = 0;
   
  uart->newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
  uart->newtio.c_cc[VMIN]     = 11;   /* this is number of data recieved */
  
  tcflush(uart->fd, TCIFLUSH);
  tcsetattr(uart->fd,TCSANOW,&uart->newtio);
  
//  tcsetattr(fd,TCSANOW,&oldtio); /* on fd close */
//  pghal_register_node// 
 

  return uart;
}

struct pghal_bus * uart_open_bus(char * address)
{
   struct uart_node * uart = NULL;
   struct pghal_bus * bus = NULL;

   uart = uart_creat_open(address);
   if (uart == NULL) 
   { 
     return NULL;
   }
   bus = &uart->bus;
   return bus;
}


void uart_node_driver_register( struct pghal_list *list)
{
  pghal_node_driver_register(list, &uart_node_driver);
}


void uart_bus_driver_register( struct pghal_list *list)
{
  pghal_bus_driver_register(list, &uart_bus_driver);
}



