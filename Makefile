CC=gcc
CFLAGS=-Isrc/ -g
LDFLAGS=-lm

DEPS_WB = 

DEPS_CHIP = 

DEPS_H =


PROGS = fmc2_config.o

OBJ_WB =
OBJ_CHIP = 

OBJ = 

INC_DIR = src/
TEST_DIR = tests/

include $(INC_DIR)/makefile.mk

DEPS_H += $(DEPS_WB) $(DEPS_CHIP)
DEPS = Makefile $(DEPS_H) $(DEPS_WB) $(DEPS_CHIP)
OBJ += $(OBJ_WB) $(OBJ_CHIP)

all: alltests

include $(TEST_DIR)/makefile.mk

TEST_PROGS = $(TEST_OBJ:.o=)

alltests: $(TEST_PROGS)


dma_to_device: dma_to_device.o
	$(CC) -lrt -o $@ $< -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D_LARGE_FILE_SOURCE

dma_to_device.o: dma_to_device.c
	$(CC) -c -std=c99 -o $@ $< -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -D_LARGE_FILE_SOURCE

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


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
	-rm fmc2_config xdma_enum_cards fmc2_test_scope fmc2_test_pattern watch_xscope ${OBJ} $(TEST_OBJ) $(TEST_PROGS)
