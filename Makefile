CC=gcc
CFLAGS=-I. -g
LDFLAGS=-lm
DEPS_H = sdbfactory.h list.h sdb_xdma.h sdb_i2c.h chip/chip_si57x.h
DEPS = Makefile $(DEPS_H)

PROGS = fmc2_config.o
OBJ = pghal.o sdb_xdma.o sdb_i2c.o chip/chip_si57x.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fmc2_config: $(OBJ) fmc2_config.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}


.PHONY: clean
clean:
	-rm fmc2_config ${OBJ}
