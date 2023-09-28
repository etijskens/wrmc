#!/bin/bash
echo
echo "Running $SLURM_JOB_NAME.slurm on $SLURM_CLUSTER_NAME/$VSC_INSTITUTE_CLUSTER"
echo "n_sruns    = $n_sruns"
echo "n_mpi/srun = $n_mpi"
echo "n_opm/mpi  = $SLURM_CPUS_PER_TASK"
echo 