
DEPS_WB += ${INC_DIR}wishbone/wb_fmc_csr.h
DEPS_WB += ${INC_DIR}wishbone/wb_gpio_raw.h
DEPS_WB += ${INC_DIR}wishbone/wb_i2c.h
DEPS_WB += ${INC_DIR}wishbone/wb_spi.h
DEPS_WB += ${INC_DIR}wishbone/xwb_scope.h
DEPS_WB += ${INC_DIR}wishbone/wb_xilinx_dna.h
DEPS_WB += ${INC_DIR}wishbone/wb_sdb_rom.h

DEPS_CHIP += ${INC_DIR}chip/chip_si57x.h
DEPS_CHIP += ${INC_DIR}chip/chip_ad9510.h
DEPS_CHIP += ${INC_DIR}chip/chip_isla216p.h

DEPS_H += ${INC_DIR}pghal_inc.h
DEPS_H += ${INC_DIR}static_offsets.h
DEPS_H += ${INC_DIR}sdb_uart.h
DEPS_H += ${INC_DIR}pghal_i2c.h
DEPS_H += ${INC_DIR}pghal_spi.h
DEPS_H += ${INC_DIR}sdb_bus.h
DEPS_H += ${INC_DIR}pghal.h list.h sdb_xdma.h
DEPS_H += ${INC_DIR}fmc/fmc_adc250m.h


OBJ_WB += ${INC_DIR}wishbone/wb_fmc_csr.o
OBJ_WB += ${INC_DIR}wishbone/wb_gpio_raw.o
OBJ_WB += ${INC_DIR}wishbone/wb_i2c.o
OBJ_WB += ${INC_DIR}wishbone/wb_spi.o
OBJ_WB += ${INC_DIR}wishbone/wb_xilinx_dna.o
OBJ_WB += ${INC_DIR}wishbone/xwb_scope.o
OBJ_WB += ${INC_DIR}wishbone/wb_sdb_rom.o

OBJ_CHIP += ${INC_DIR}chip/chip_ad9510.o
OBJ_CHIP += ${INC_DIR}chip/chip_si57x.o
OBJ_CHIP += ${INC_DIR}chip/chip_isla216p.o

OBJ += ${INC_DIR}pghal.o
OBJ += ${INC_DIR}sdb_bus.o
OBJ += ${INC_DIR}sdb_xdma.o
OBJ += ${INC_DIR}pghal_spi.o
OBJ += ${INC_DIR}pghal_i2c.o
OBJ += ${INC_DIR}sdb_uart.o

OBJ += ${INC_DIR}fmc/fmc_adc250m.o

