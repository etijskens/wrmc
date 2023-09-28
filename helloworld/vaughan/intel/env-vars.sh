#!/bin/bash
# control placement
export OMP_PROC_BIND=true

# number of mpi processes per srun
export n_mpi=$(( ${SLURM_NTASKS} / n_sruns ))
