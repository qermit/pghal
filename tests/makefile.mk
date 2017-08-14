
TEST_OBJ += $(TEST_DIR)xdma_enum_cards.o

$(TEST_DIR)xdma_enum_cards: $(OBJ) $(TEST_DIR)xdma_enum_cards.o
	gcc -o $@ $^ $(CFLAGS) ${LDFLAGS}

