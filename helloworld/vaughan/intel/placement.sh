#!/bin/bash

# Start echoing the executed commands to stderr
echo "Executing $VSC_INSTITUTE_CLUSTER/$toolchain/placement.sh" 1>&2
echo "[set -x]" 1>&2
echo  1>&2
set -x

# control placement
export OMP_PROC_BIND=true
export I_MPI_PIN_DOMAIN=omp,compact
# set | grep "PMI_\|MPI_" | sort

# Stop echoing the executed commands to stderr
set +x
