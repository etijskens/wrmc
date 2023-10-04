#!/bin/bash
#SBATCH --job-name=hello_1x1x8
#SBATCH --error=%x-%j.e
#SBATCH --output=%x-%j.o
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --time=00:05:00
#SBATCH --account=astaff

# get the toolchain from the name of the current directory
toolchain=${PWD##*/}
. ../env-modules.sh ${toolchain} && ml
. ../job-config.sh 1
. ./placement.sh

srun --quiet -n${n_mpi_ranks_per_srun} -c${SLURM_CPUS_PER_TASK} --exclusive --unbuffered ./bin/hello