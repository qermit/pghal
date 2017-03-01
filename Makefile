CC=gcc
CFLAGS=-I. -g
DEPS_H = sdbfactory.h list.h sdb_xdma.h sdb_i2c.h chip/chip_si57x.h
DEPS = Makefile $(DEPS_H)
OBJ = sdbfactory.o sdb_xdma.o sdb_i2c.o chip/chip_si57x.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sdbfactory: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
