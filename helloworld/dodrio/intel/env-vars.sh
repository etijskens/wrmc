#!/bin/bash

# Start echoing the executed commands to stderr
echo ">> $VSC_INSTITUTE_CLUSTER/$toolchain/env-vars.sh" 1>&2
set -x

# control placement
export OMP_PROC_BIND=true
export I_MPI_PIN_DOMAIN=omp,compact

# Stop echoing the executed commands to stderr
set +x