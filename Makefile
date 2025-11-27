// Makefile for building and testing hash table implementations
# Usage:
#   make              - Build all executables
#   make test         - Run all unit tests
#   make bench        - Run benchmark comparison
#   make csv          - Output CSV for plotting
#   make clean        - Remove build artifacts

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lm

# Source directory
SRC = src

# Executables (built in project root)
TARGETS = test_cuckoo test_chained test_linear benchmark

.PHONY: all test bench csv clean

all: $(TARGETS)

# === Test executables ===

test_cuckoo: $(SRC)/test_cuckoo.c $(SRC)/cuckoo_hash.c $(SRC)/cuckoo_hash.h
	$(CC) $(CFLAGS) -o $@ $(SRC)/test_cuckoo.c $(SRC)/cuckoo_hash.c $(LDFLAGS)

test_chained: $(SRC)/test_chained.c $(SRC)/chained_hash.c $(SRC)/chained_hash.h
	$(CC) $(CFLAGS) -o $@ $(SRC)/test_chained.c $(SRC)/chained_hash.c $(LDFLAGS)

test_linear: $(SRC)/test_linear.c $(SRC)/linear_probing.c $(SRC)/linear_probing.h
	$(CC) $(CFLAGS) -o $@ $(SRC)/test_linear.c $(SRC)/linear_probing.c $(LDFLAGS)

# === Benchmark executable ===

benchmark: $(SRC)/benchmark.c $(SRC)/cuckoo_hash.c $(SRC)/chained_hash.c $(SRC)/linear_probing.c
	$(CC) $(CFLAGS) -o $@ $(SRC)/benchmark.c $(SRC)/cuckoo_hash.c $(SRC)/chained_hash.c $(SRC)/linear_probing.c $(LDFLAGS)

# === Run targets ===

test: test_cuckoo test_chained test_linear
	@echo ""
	@echo "=========================================="
	@echo "         RUNNING ALL UNIT TESTS"
	@echo "=========================================="
	@echo ""
	./test_cuckoo
	@echo ""
	./test_chained
	@echo ""
	./test_linear
	@echo ""
	@echo "=========================================="
	@echo "         ALL TESTS COMPLETED"
	@echo "=========================================="

bench: benchmark
	./benchmark

csv: benchmark
	./benchmark csv > results.csv
	@echo "Results saved to results.csv"

# === Cleanup ===

clean:
	rm -f $(TARGETS) results.csv

# === Debug build ===

debug: CFLAGS += -g -DDEBUG -fsanitize=address
debug: clean all