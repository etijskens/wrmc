#!/bin/bash
#SBATCH --job-name=hello_4x1x8
#SBATCH --error=%x-%j.e
#SBATCH --output=%x-%j.o
#SBATCH --nodes=1
#SBATCH --exclusive#SBATCH --ntasks=4
#SBATCH --cpus-per-task=8
#SBATCH --time=00:05:00
#SBATCH --account=astaff

toolchain=intel
. ../env-modules.sh ${toolchain} && ml vsc-mympirun && ml
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mympirun --hybrid 1 ../intel/bin/hello 0 &
mympirun --hybrid 1 ../intel/bin/hello 1 &
mympirun --hybrid 1 ../intel/bin/hello 2 &
mympirun --hybrid 1 ../intel/bin/hello 3 &

wait
