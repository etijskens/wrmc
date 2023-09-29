#!/bin/bash

# Start echoing the executed commands to stderr
echo "## $0" 1>&2
set -x

# control placement
export OMP_PROC_BIND=true

# number of mpi processes per srun
export n_mpi=$(( ${SLURM_NTASKS} / n_sruns ))

# Stop echoing the executed commands to stderr
set +x

