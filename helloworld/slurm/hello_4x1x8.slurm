#!/bin/bash
#SBATCH --job-name=hello_4x1x8
#SBATCH --error=%x-%j.e
#SBATCH --output=%x-%j.o
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=8
#SBATCH --time=00:05:00

# get the toolchain from the name of the current directory
toolchain=${PWD##*/}
. ../env-modules.sh ${toolchain}
. ../job-config.sh 4
. ./placement.sh

srun --quiet -n${n_mpi_ranks_per_srun} -c${SLURM_CPUS_PER_TASK} --exclusive --unbuffered ./bin/hello 0 &
srun --quiet -n${n_mpi_ranks_per_srun} -c${SLURM_CPUS_PER_TASK} --exclusive --unbuffered ./bin/hello 1 &
srun --quiet -n${n_mpi_ranks_per_srun} -c${SLURM_CPUS_PER_TASK} --exclusive --unbuffered ./bin/hello 2 &
srun --quiet -n${n_mpi_ranks_per_srun} -c${SLURM_CPUS_PER_TASK} --exclusive --unbuffered ./bin/hello 3 &
wait
