CFLAGS=-fPIC -Wall -Wextra -O2
LDFLAGS=-shared
TARGET_LIB=libe4c.so
SRC_DIR=src
OBJ_DIR=obj
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
OBJ_FILES=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

.PHONY: all clean

all: $(TARGET_LIB)

$(TARGET_LIB): $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -f $(TARGET_LIB) $(OBJ_FILES)
	rmdir $(OBJ_DIR)

