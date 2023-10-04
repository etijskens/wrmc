#!/bin/bash
#SBATCH --job-name=hello_1x2x4
#SBATCH --error=%x-%j.e
#SBATCH --output=%x-%j.o
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=4
#SBATCH --time=00:05:00
#SBATCH --account=astaff

toolchain=intel
. ../env-modules.sh ${toolchain} && ml vsc-mympirun && ml
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mympirun --hybrid 2 ../intel/bin/hello