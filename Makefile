# Hash Map Implementations Makefile
# CS 5008 - Siddharth Kakked

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
DEBUG_FLAGS = -g -DDEBUG

# Source directory
SRC_DIR = src

# Hash map implementation sources
IMPL_SRCS = $(SRC_DIR)/chained.c $(SRC_DIR)/linear_probing.c $(SRC_DIR)/cuckoo.c

# Test sources
TEST_SRCS = $(SRC_DIR)/test_utils.c $(SRC_DIR)/test_correctness.c \
            $(SRC_DIR)/test_benchmarks.c $(SRC_DIR)/test_main.c

# All sources and objects
SRCS = $(IMPL_SRCS) $(TEST_SRCS)
OBJS = $(SRCS:.c=.o)
TARGET = test_hashmaps

# Default target: build everything
all: $(TARGET)

# Link object files into executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c files to .o files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Debug build with symbols
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all

# Remove build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Build and run all tests
run: $(TARGET)
	./$(TARGET)

# Run only correctness tests
test: $(TARGET)
	./$(TARGET) --correctness

# Run only benchmarks
bench: $(TARGET)
	./$(TARGET) --benchmarks

.PHONY: all clean debug run test bench