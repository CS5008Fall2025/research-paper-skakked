// Makefile for Hash Table Implementations and Benchmarks 
# Usage:
#   make              - Build all executables
#   make test         - Run all unit tests
#   make bench        - Run benchmark comparison
#   make csv          - Output CSV for plotting
#   make clean        - Remove build artifacts

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lm

# Executables
TARGETS = test_cuckoo test_chained test_linear benchmark

# Source files
CUCKOO_SRC = cuckoo_hash.c
CHAINED_SRC = chained_hash.c
LINEAR_SRC = linear_probing.c

.PHONY: all test bench csv clean test_all

all: $(TARGETS)

# === Test executables ===

test_cuckoo: test_cuckoo.c $(CUCKOO_SRC) cuckoo_hash.h
	$(CC) $(CFLAGS) -o $@ test_cuckoo.c $(CUCKOO_SRC) $(LDFLAGS)

test_chained: test_chained.c $(CHAINED_SRC) chained_hash.h
	$(CC) $(CFLAGS) -o $@ test_chained.c $(CHAINED_SRC) $(LDFLAGS)

test_linear: test_linear.c $(LINEAR_SRC) linear_probing.h
	$(CC) $(CFLAGS) -o $@ test_linear.c $(LINEAR_SRC) $(LDFLAGS)

# === Benchmark executable ===

benchmark: benchmark.c $(CUCKOO_SRC) $(CHAINED_SRC) $(LINEAR_SRC)
	$(CC) $(CFLAGS) -o $@ benchmark.c $(CUCKOO_SRC) $(CHAINED_SRC) $(LINEAR_SRC) $(LDFLAGS)

# === Run targets ===

test: test_all

test_all: test_cuckoo test_chained test_linear
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
	rm -f $(TARGETS) *.o results.csv

# === Debug build ===

debug: CFLAGS += -g -DDEBUG -fsanitize=address
debug: clean all