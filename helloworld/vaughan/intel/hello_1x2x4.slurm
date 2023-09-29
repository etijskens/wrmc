#!/bin/bash
#SBATCH --job-name=hello_1x2x4
#SBATCH --error=%x-%j.e
#SBATCH --output=%x-%j.o
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=4
#SBATCH --time=00:05:00

. ../env-modules.sh foss
. ../job-config.sh 1
. ./placement.sh

srun --quiet -n${n_mpi_ranks_per_srun} -c${SLURM_CPUS_PER_TASK} --exclusive --unbuffered ./bin/hello 0

