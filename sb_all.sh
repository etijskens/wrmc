#!/bin/bash
for f in ../../slurm/*.slurm
do
    echo $f
    sbatch $f
done
