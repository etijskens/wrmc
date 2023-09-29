#!/bin/bash

# $1 = the number of simultaneous sruns in the job script
if [ "$1" = "" ]
then 
    echo "ERROR: number of simultaneous sruns not provided" 1>&2
    return 1
fi

export n_sruns=$1
# the number of mpi processes per srun
export n_mpi_ranks_per_srun=$(( ${SLURM_NTASKS} / n_sruns ))
# the number of OpenMP threads per MPI rank
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

# Print cjob config info:
echo
echo "Running ${SLURM_JOB_NAME}.slurm on ${SLURM_CLUSTER_NAME}:${VSC_INSTITUTE_CLUSTER}"
echo "#sruns                       = ${n_sruns}"
echo "#MPI ranks per srun          = ${n_mpi_ranks_per_srun}"
echo "#OpenMP threads per MPI rank = ${SLURM_CPUS_PER_TASK}"
echo 