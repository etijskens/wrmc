#!/bin/bash
#SBATCH --job-name=xthi_2x4x2
#SBATCH --error=%x-%j.e
#SBATCH --output=%x-%j.o
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=2
#SBATCH --time=00:05:00

module --force purge
module load calcua/2022a
module load intel
module load hwloc
# module load PMIx

PATH=./build:$PATH

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
export OMP_PROC_BIND=true
export I_MPI_PIN_DOMAIN=omp,compact

srun --quiet -n4 -c2 xthi A 10 &
srun --quiet -n4 -c2 xthi B 10 &
wait