#include "pghal_inc.h"

#include "pghal_spi.h"

void pghal_spi_bus_init(struct pghal_spi *spi)
{
  struct pghal_bus * bus = & spi->bus;
  
  INIT_LIST_HEAD(&bus->module_list);
}

void spi_chip_register(struct pghal_spi *spi, uint8_t spi_address)
{
  
}

// abstract call from struct pghal_bus level
static int _pghal_spi_write_read(struct pghal_bus * bus, struct pghal_address * address, struct pghal_op_rw * op)
{
  struct pghal_spi * spi = (struct pghal_spi *) (((void *) bus) - offsetof(struct pghal_spi, bus));
  struct pghal_spi_address  * spi_address = (struct pghal_spi_address *) (((void *) address) - offsetof(struct pghal_spi_address, address));

//  return pghal_spi_write_read(spi, spi_address->ss_id, op);


}

int pghal_spi_a16_write(struct pghal_spi * spi, uint8_t ss_id, uint16_t address, size_t len, uint8_t * ptr)
{

  uint8_t addr_op[2] = { ((address >> 8)& 0x001F), (address & 0x00FF) };
  struct pghal_op_rw op = { .start = 1, .stop = 0, .granularity = 8, .wr_offset = 0, .rd_offset = 0, .rd_ptr = NULL, .rd_ptr_end = NULL, .wr_ptr = addr_op, .wr_ptr_end = addr_op+2 };

  if (len == 2) addr_op[0] |= 0x20;
  if (len == 3) addr_op[0] |= 0x40;
  if (len >  3) addr_op[0] |= 0x60;

  spi->write_read(spi, ss_id, &op);

  op.wr_ptr = ptr;
  op.wr_ptr_end = ptr + len;
  op.stop = 1;
  op.start = 0;

  spi->write_read(spi, ss_id, &op);
 
  return 0;
}

int pghal_spi_a16_read(struct pghal_spi * spi, uint8_t ss_id, uint16_t address, size_t len, uint8_t * ptr)
{

  uint8_t addr_op[2] = { ((address >> 8)& 0x001F), (address & 0x00FF) };
  struct pghal_op_rw op = { .start = 1, .stop = 1, .granularity = 8, .wr_offset = 0, .rd_offset = 2, .rd_ptr = ptr, .rd_ptr_end = ptr+len, .wr_ptr = addr_op, .wr_ptr_end = addr_op+2 };

  if (len == 2) addr_op[0] |= 0x20;
  if (len == 3) addr_op[0] |= 0x40;
  if (len >  3) addr_op[0] |= 0x60;

  addr_op[0] |= 0x80;

  spi->write_read(spi, ss_id, &op);

  return 0;

}
// direct call
int pghal_spi_write_read(struct pghal_spi * spi, uint8_t ss_id, struct pghal_op_rw * op)
{
  return spi->write_read(spi, ss_id, op);
}


void pghal_spi_set_params(struct pghal_spi * spi, uint8_t flags)
{
  spi->flags = flags;
}

