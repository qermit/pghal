
TEST_OBJ += $(TEST_DIR)xdma_enum_cards.o
TEST_OBJ += $(TEST_DIR)fmc2_config.o
TEST_OBJ += $(TEST_DIR)pghal_wr.o

$(TEST_DIR)pghal_wr: $(OBJ) $(TEST_DIR)pghal_wr.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}

$(TEST_DIR)xdma_enum_cards: $(OBJ) $(TEST_DIR)xdma_enum_cards.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}

$(TEST_DIR)fmc2_config: $(OBJ) $(TEST_DIR)fmc2_config.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}

