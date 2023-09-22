#!/bin/bash
#SBATCH --job-name=xthi_2x8x4
#SBATCH --error=%x-%j.e
#SBATCH --output=%x-%j.o
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks=16
#SBATCH --cpus-per-task=4
#SBATCH --time=00:05:00

module --force purge
module load calcua/2022a
module load intel
unset I_MPI_HYDRA_BOOTSTRAP
unset I_MPI_HYDRA_RMK
unset I_MPI_PIN_RESPECT_CPUSET
unset I_MPI_PMI_LIBRARY
unset I_MPI_PMI

unset SLURM_MPI_TYPE

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
# export OMP_PROC_BIND=true
# export I_MPI_PIN_DOMAIN=omp,compact

export n=$((${SLURM_NTASKS} / 2))
echo "n=$n"

srun --quiet -n${n} -c${SLURM_CPUS_PER_TASK} --exclusive --unbuffered ../build/xthi A 10 &
srun --quiet -n${n} -c${SLURM_CPUS_PER_TASK} --exclusive --unbuffered ../build/xthi B 10 &
wait