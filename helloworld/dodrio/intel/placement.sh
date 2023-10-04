#!/bin/bash

# Start echoing the executed commands to stderr
echo "Executing $VSC_INSTITUTE_CLUSTER/$toolchain/placement.sh" 1>&2
echo "[set -x]" 1>&2
echo  1>&2
set -x

# control placement
export OMP_PROC_BIND=true
export I_MPI_PIN_DOMAIN=omp,compact

export I_MPI_HYDRA_BOOTSTRAP=slurm
export I_MPI_HYDRA_RMK=slurm
export I_MPI_PIN_RESPECT_CPUSET=0
export I_MPI_PMI_LIBRARY=/usr/lib64/slurmpmi/libpmi2.so
export I_MPI_PMI=pmi2

set | grep "PMI_\|MPI_" | sort

# Stop echoing the executed commands to stderr
set +x
