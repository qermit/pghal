#include "pghal_inc.h"

#include "sdb_bus.h"
#include "pghal_spi.h"
#include "wb_spi.h"

#define WB_SPI_RXTX_0 0x00
#define WB_SPI_RXTX_1 0x04
#define WB_SPI_RXTX_2 0x08
#define WB_SPI_RXTX_3 0x0C
#define WB_SPI_CTRL   0x10
#define WB_SPI_DIVIDE 0x14
#define WB_SPI_SS     0x18
#define WB_SPI_RES0   0x1C


// generic implementation write/read transaction
static int wb_spi_write_read(struct pghal_spi * spi, uint8_t spi_ss, struct pghal_op_rw * op){
//  printf("wb_spi_write_read, =====\n");
  
  struct wb_spi * wb_spi = (struct wb_spi *) (((void *) spi) - offsetof(struct wb_spi, spi));

  struct sdb_node_address reg_spi_tx_0;
  struct sdb_node_address reg_spi_ctrl;
  struct sdb_node_address reg_spi_ss;
  memcpy(&reg_spi_tx_0, &wb_spi->sdb.address, sizeof(struct sdb_node_address));
  memcpy(&reg_spi_ctrl, &wb_spi->sdb.address, sizeof(struct sdb_node_address));
  memcpy(&reg_spi_ss, &wb_spi->sdb.address, sizeof(struct sdb_node_address));
  reg_spi_tx_0.sdb_address = reg_spi_tx_0.sdb_address  + WB_SPI_RXTX_0;
  reg_spi_ctrl.sdb_address = reg_spi_ctrl.sdb_address  + WB_SPI_CTRL;
  reg_spi_ss.sdb_address   = reg_spi_ss.sdb_address  + WB_SPI_SS;


  int transfer_len = 0;
  int i;
  uint32_t CR;
  int wr_length = (op->wr_ptr_end - op->wr_ptr);
  int rd_length = (op->rd_ptr_end - op->rd_ptr);
  
  if (wr_length > 0) wr_length = wr_length + op->wr_offset ;
  if (rd_length > 0) rd_length = rd_length + op->rd_offset ;
 
  if (wr_length > transfer_len) transfer_len = wr_length;
  if (rd_length > transfer_len) transfer_len = rd_length;
  

  if ((spi->flags & (PGHAL_SPI_CPHA_BIT | PGHAL_SPI_CPHA_BIT)) == 0) {
      CR =  0x400;
  }
  CR |= 0x0008; 
  pghal_bus_write(wb_spi->sdb.bus, &reg_spi_ctrl.address , 1*sizeof(uint32_t), &CR);
    
  //  data_w[0] = 0x0108 | (( spi->tx_on_rising == 1 ) ? 0x0400 : 0) | (( spi->tx_on_rising == 1 ) ? 0x200 : 0);

  uint32_t data_w[1] = { 1 << spi_ss } ;
  uint8_t * rd_ptr = op->rd_ptr;
  if (op->start) pghal_bus_write(wb_spi->sdb.bus, &reg_spi_ss.address , 1*sizeof(uint32_t), &data_w);

  for(i = 0; i< transfer_len; i++){
    if (i >= op->wr_offset && i < wr_length) {
       uint8_t tx_byte = ((uint8_t *)op->wr_ptr)[i-op->wr_offset];
       data_w[0] = tx_byte;
    } else {
       data_w[0] = 0xFF;
    }
    pghal_bus_write(wb_spi->sdb.bus, &reg_spi_tx_0.address , 1*sizeof(uint32_t), data_w);
  
    
    //data_w[0] = 0x0108 | (( spi->tx_on_rising == 1 ) ? 0x0400 : 0) | (( spi->tx_on_rising == 1 ) ? 0x200 : 0);
    CR |= 0x100; // go
    pghal_bus_write(wb_spi->sdb.bus, &reg_spi_ctrl.address , 1*sizeof(uint32_t), &CR);
   
    pghal_bus_read(wb_spi->sdb.bus, &reg_spi_ctrl.address , 1*sizeof(uint32_t), data_w);
    while(data_w[0] & 0x0100) {
      pghal_bus_read(wb_spi->sdb.bus, &reg_spi_ctrl.address , 1*sizeof(uint32_t), data_w);
    }
    
    if (i >= op->rd_offset && i < rd_length) {
      pghal_bus_read(wb_spi->sdb.bus, &reg_spi_tx_0.address , 1*sizeof(uint32_t), data_w);
      rd_ptr[i-op->rd_offset] = data_w[0];
//      pghal_bus_read(wb_spi->sdb.bus, &reg_spi_ctrl.address , 1*sizeof(uint32_t), rd_ptr+i-op->rd_offset);
    
    } 

  }
   
  data_w[0] = 0 ;
  if (op->stop) pghal_bus_write(wb_spi->sdb.bus, &reg_spi_ss.address , 1*sizeof(uint32_t), &data_w);

  return 0;
//  uint32_t wb_address = wb_i2c->sdb.address.sdb_address;
//  struct sdb_node_address * tmp_address = &wb_i2c->tmp_address;
//  uint32_t data_w;
//  uint32_t SR;

//  pghal_bus_write(wb_spi->sdb.bus, struct pghal_address * addr, size_t wr_len, void *wr_ptr )
//  tmp_address->sdb_address = wb_address + WB_I2C_REG_SR_CR ; pghal_bus_read(bus, &tmp_address->address , 1*sizeof(uint32_t), &SR);

}

static int wb_spi_a16_write(struct pghal_spi * spi, uint8_t ss_id, uint16_t address, size_t len, uint8_t * ptr)
{

}
static int wb_spi_a16_read(struct pghal_spi * spi, uint8_t ss_id, uint16_t address, size_t len, uint8_t * ptr)
{

}

struct wb_spi * wb_spi_init(struct pghal_bus * bus, uint32_t bus_address)
{
  struct wb_spi * spi = NULL;
  spi = (struct wb_spi *) pghal_alloc((sizeof(struct wb_spi)));

  sdb_node_init(&spi->sdb, bus, bus_address);
  pghal_spi_bus_init(&spi->spi);
  spi->spi.write_read = wb_spi_write_read;

  struct sdb_node_address reg_spi_divide;
  memcpy(&reg_spi_divide, &spi->sdb.address, sizeof(struct sdb_node_address));
  reg_spi_divide.sdb_address = reg_spi_divide.sdb_address + WB_SPI_DIVIDE;
  
  uint32_t data_w[1] = { 0x50 };
  pghal_bus_write(bus, &reg_spi_divide.address , 1*sizeof(uint32_t), &data_w); 

  return spi;
}

int wb_spi_write_read_direct(struct wb_spi * wb_spi, uint8_t ss_id, struct pghal_op_rw * op){
   return wb_spi_write_read(&wb_spi->spi, ss_id, op);
}

