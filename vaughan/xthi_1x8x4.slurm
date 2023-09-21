#!/bin/bash
#SBATCH --job-name=xthi_1x8x4
#SBATCH --error=%x-%j.e
#SBATCH --output=%x-%j.o
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=4
#SBATCH --time=00:05:00

module --force purge
module load calcua/2022a
module load intel

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
# export OMP_PROC_BIND=true
# export I_MPI_PIN_DOMAIN=omp,compact

export n=${SLURM_NTASKS}
echo "n=$n"

srun --quiet -n${n} -c${SLURM_CPUS_PER_TASK} --exclusive --unbuffered ../build/xthi A 10 -v