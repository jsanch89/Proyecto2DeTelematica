#!/usr/bin/bash
mpirun -np $1 -f hosts_mpi ./algo-mpi-pu
