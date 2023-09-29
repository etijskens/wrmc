# number of mpi processes per srun
export n_sruns=$1
export n_mpi_per_srun=$(( ${SLURM_NTASKS} / n_sruns ))
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
echo
echo "Running $SLURM_JOB_NAME.slurm on $SLURM_CLUSTER_NAME/$VSC_INSTITUTE_CLUSTER"
echo "#sruns                       = $n_sruns"
echo "#MPI ranks per srun          = $n_mpi_per_srun"
echo "#OpenMP threads per MPI rank = $SLURM_CPUS_PER_TASK"
echo 