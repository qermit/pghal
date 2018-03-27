
TEST_OBJ += $(TEST_DIR)xdma_enum_cards.o
TEST_OBJ += $(TEST_DIR)fmc2_config.o
TEST_OBJ += $(TEST_DIR)pghal_wr.o
TEST_OBJ += $(TEST_DIR)pghal_xvc.o
TEST_OBJ += $(TEST_DIR)watch_xscope.o
TEST_OBJ += $(TEST_DIR)sim_config.o
TEST_OBJ += $(TEST_DIR)raw_config.o

$(TEST_DIR)watch_xscope: $(OBJ) $(TEST_DIR)watch_xscope.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}

$(TEST_DIR)pghal_wr: $(OBJ) $(TEST_DIR)pghal_wr.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}

$(TEST_DIR)xdma_enum_cards: $(OBJ) $(TEST_DIR)xdma_enum_cards.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}

$(TEST_DIR)sim_config: $(OBJ) $(TEST_DIR)sim_config.o
	gcc -g -o $@ $^ $(CFLAGS) ${LDFLAGS}

$(TEST_DIR)fmc2_config: $(OBJ) $(TEST_DIR)fmc2_config.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}

$(TEST_DIR)pghal_xvc: $(OBJ) $(TEST_DIR)pghal_xvc.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}

$(TEST_DIR)raw_config: $(OBJ) $(TEST_DIR)raw_config.o
	gcc -g -o $@ $^ $(CFLAGS) ${LDFLAGS}
