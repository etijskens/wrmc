#!/bin/bash
#SBATCH --job-name=hello_4x2x4
#SBATCH --error=%x-%j.e
#SBATCH --output=%x-%j.o
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=4
#SBATCH --time=00:05:00
#SBATCH --account=astaff

toolchain=intel
. ../env-modules.sh ${toolchain} && ml vsc-mympirun && ml
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mympirun --universe 2 ../intel/bin/hello 0 &
mympirun --universe 2 ../intel/bin/hello 1 &
mympirun --universe 2 ../intel/bin/hello 2 &
mympirun --universe 2 ../intel/bin/hello 3 &

wait
