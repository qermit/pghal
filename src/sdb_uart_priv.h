#ifndef __SDB_UART_PRIV_H__
#define __SDB_UART_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

struct uart_node_address {
  struct pghal_address address; 

  char * dev; // pointer to dev
};

struct uart_node {
  struct pghal_node node; // node information, it should hold one pghal_node_address embedded into uart_node_address
  struct pghal_bus bus; // bus exported (sdb_bus in case, it holds pointer to druver (and supported OPs)

  struct uart_node_address address;
    
  // all data required to do write/read ops 
  int fd;
  int debug;
  struct termios oldtio;
  struct termios newtio;

};

struct uart_node * uart_create_open(char * address);
uint32_t uart_read_direct(struct uart_node * uart, uint32_t addr);
void uart_write_direct(struct uart_node * uart, uint32_t addr, uint32_t value);


#endif
