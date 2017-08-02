CC=gcc
CFLAGS=-I. -g
LDFLAGS=-lm

DEPS_WB = 
DEPS_WB += wishbone/wb_gpio_raw.h
DEPS_WB += wishbone/wb_i2c.h
DEPS_WB += wishbone/wb_spi.h
DEPS_WB += wishbone/xwb_scope.h
DEPS_WB += wishbone/wb_xilinx_dna.h

DEPS_CHIP = 
DEPS_CHIP += chip/chip_si57x.h
DEPS_CHIP += chip/chip_ad9510.h
DEPS_CHIP += chip/chip_isla216p.h

DEPS_H = pghal_inc.h
DEPS_H += static_offsets.h
DEPS_H += pghal.h list.h sdb_xdma.h $(DEPS_WB) $(DEPS_CHIP)
DEPS_H += pghal_i2c.h
DEPS_H += pghal_spi.h
DEPS_H += sdb_bus.h
DEPS = Makefile $(DEPS_H)

PROGS = fmc2_config.o
OBJ_WB = wishbone/wb_gpio_raw.o
OBJ_WB += wishbone/wb_i2c.o
OBJ_WB += wishbone/wb_spi.o
OBJ_WB += wishbone/wb_xilinx_dna.o
OBJ_WB += wishbone/xwb_scope.o
OBJ_CHIP = 
OBJ_CHIP += chip/chip_ad9510.o
OBJ_CHIP += chip/chip_si57x.o
OBJ_CHIP += chip/chip_isla216p.o

OBJ = pghal.o
OBJ += sdb_bus.o
OBJ += sdb_xdma.o
OBJ += pghal_spi.o
OBJ += pghal_i2c.o
OBJ += $(OBJ_WB) $(OBJ_CHIP)

all: fmc2_config fmc2_test_pattern fmc2_test_scope xdma_enum_cards

dma_to_device: dma_to_device.o
	$(CC) -lrt -o $@ $< -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D_LARGE_FILE_SOURCE

dma_to_device.o: dma_to_device.c
	$(CC) -c -std=c99 -o $@ $< -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D_LARGE_FILE_SOURCE

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

xdma_enum_cards: $(OBJ) xdma_enum_cards.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}

fmc2_test_scope: $(OBJ) fmc2_test_scope.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}
fmc2_config: $(OBJ) fmc2_config.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}
fmc2_test_pattern: $(OBJ) fmc2_test_pattern.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}

watch_xscope:  $(OBJ) watch_xscope.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}


.PHONY: clean
clean:
	-rm fmc2_config ${OBJ}
