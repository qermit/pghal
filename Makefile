CC=gcc
CFLAGS=-I. -g
LDFLAGS=-lm
#DEPS_WB = wishbone/wb_gpio_raw.h wishbone/wb_spi.h
DEPS_WB = wishbone/wb_gpio_raw.h
DEPS_CHIP = chip/chip_si57x.h chip/chip_ad9510.h
DEPS_H = pghal.h list.h sdb_xdma.h sdb_spi.h $(DEPS_WB) $(DEPS_CHIP)
DEPS_H += sdb_bus.h
DEPS_H += sdb_i2c.h
DEPS = Makefile $(DEPS_H)

PROGS = fmc2_config.o
#OBJ_WB = wishbone/wb_gpio_raw.o wishbone/wb_spi.o
OBJ_WB = wishbone/wb_gpio_raw.o
OBJ_CHIP = 
#OBJ_CHIP += chip/chip_ad9510.o
#OBJ_CHIP += chip/chip_si57x.o
OBJ = pghal.o sdb_xdma.o sdb_spi.o $(OBJ_WB) $(OBJ_CHIP)
OBJ += sdb_i2c.o
OBJ += sdb_bus.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fmc2_config: $(OBJ) fmc2_config.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}


.PHONY: clean
clean:
	-rm fmc2_config ${OBJ}
