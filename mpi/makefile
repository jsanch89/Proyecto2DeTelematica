# Makefile

MPIC++ = $(shell which mpic++)
MPIROOT = $(dir $(MPIC++))

C++ = mpic++

# Optimizations

CFLAGS = -O2 -w -Wall

BIN = algo-mpi-pu

all: $(BIN)

algo-mpi-pu: algo.cpp
	$(C++) $(CFLAGS) algo.cpp -o algo-mpi-pu
clean:
	rm -rf $(BIN) *~ *.o
