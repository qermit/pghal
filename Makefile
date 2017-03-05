CC=gcc
CFLAGS=-I. -g
LDFLAGS=-lm
#DEPS_WB = wishbone/wb_gpio_raw.h wishbone/wb_spi.h
DEPS_WB = wishbone/wb_gpio_raw.h
DEPS_WB = wishbone/wb_i2c.h
DEPS_WB = wishbone/wb_spi.h
DEPS_CHIP = chip/chip_si57x.h chip/chip_ad9510.h
DEPS_H = pghal_inc.h
DEPS_H += pghal.h list.h sdb_xdma.h $(DEPS_WB) $(DEPS_CHIP)
DEPS_H += pghal_i2c.h
DEPS_H += pghal_spi.h
DEPS_H += sdb_bus.h
DEPS = Makefile $(DEPS_H)

PROGS = fmc2_config.o
#OBJ_WB = wishbone/wb_gpio_raw.o wishbone/wb_spi.o
OBJ_WB = wishbone/wb_gpio_raw.o
OBJ_WB += wishbone/wb_i2c.o
OBJ_WB += wishbone/wb_spi.o
OBJ_CHIP = 
#OBJ_CHIP += chip/chip_ad9510.o
#OBJ_CHIP += chip/chip_si57x.o
OBJ = pghal.o
OBJ += sdb_bus.o
OBJ += sdb_xdma.o
OBJ += pghal_spi.o
OBJ += pghal_i2c.o
OBJ += $(OBJ_WB) $(OBJ_CHIP)


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fmc2_config: $(OBJ) fmc2_config.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}


.PHONY: clean
clean:
	-rm fmc2_config ${OBJ}
