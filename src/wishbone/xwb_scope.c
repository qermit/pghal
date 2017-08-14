#include "pghal_inc.h"

#include "sdb_bus.h"
#include "xwb_scope.h"

#define REG_CSR        0x00
#define REG_ADDR_PTR   0x04
#define REG_ADDR_START 0x08
#define REG_ADDR_END   0x0C


struct xwb_scope * xwb_scope_create_direct(struct pghal_bus * bus, uint32_t bus_address) {
  struct xwb_scope * scope = NULL;
  scope = (struct xwb_scope *) pghal_alloc((sizeof(struct xwb_scope)));

  scope->sdb.bus = bus;
  scope->sdb.address.sdb_address =  bus_address;
}

void xwb_scope_single_shot(struct xwb_scope * scope)
{
  uint32_t data_w[1] = {XWB_SCOPE_CSR0_SINGLE_SHOT};
  struct sdb_node_address reg_csr0;
  memcpy(&reg_csr0, &scope->sdb.address, sizeof(struct sdb_node_address));

  pghal_bus_write(scope->sdb.bus, &reg_csr0.address , 1*sizeof(uint32_t), data_w);
}

void xwb_scope_single_reset(struct xwb_scope * scope)
{
  uint32_t data_w[1] = {XWB_SCOPE_CSR0_RESET};
  struct sdb_node_address reg_csr0;
  memcpy(&reg_csr0, &scope->sdb.address, sizeof(struct sdb_node_address));

  pghal_bus_write(scope->sdb.bus, &reg_csr0.address , 1*sizeof(uint32_t), data_w);
}

uint32_t xwb_scope_get_csr(struct xwb_scope * scope){
  uint32_t data_r[1];
  struct sdb_node_address reg_csr0;
  memcpy(&reg_csr0, &scope->sdb.address, sizeof(struct sdb_node_address));

  pghal_bus_read(scope->sdb.bus, &reg_csr0.address, 1*sizeof(uint32_t), data_r);

  return data_r[0];
}


void   xwb_scope_registers_download(struct xwb_scope * scope)
{

  struct sdb_node_address reg_csr;
  memcpy(&reg_csr, &scope->sdb.address, sizeof(struct sdb_node_address));

  pghal_bus_read(scope->sdb.bus, &reg_csr.address, sizeof(struct xwb_scope_csr), &scope->reg);

}


void xwb_scope_set_address_range(struct xwb_scope * scope, uint32_t start, uint32_t size){
  uint32_t data[2] = {start, start + size - 1};
  struct sdb_node_address reg_csr;
  memcpy(&reg_csr, &scope->sdb.address, sizeof(struct sdb_node_address));
  reg_csr.sdb_address += 8;
  pghal_bus_write(scope->sdb.bus, &reg_csr.address, 2*sizeof(uint32_t), data);
  xwb_scope_registers_download(scope); 
} 

void xwb_scope_stop(struct xwb_scope * scope)
{
  uint32_t data_w[1] = {0x00};
  struct sdb_node_address reg_csr0;
  memcpy(&reg_csr0, &scope->sdb.address, sizeof(struct sdb_node_address));

  pghal_bus_write(scope->sdb.bus, &reg_csr0.address , 1*sizeof(uint32_t), data_w);
}

