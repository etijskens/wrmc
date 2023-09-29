#!/bin/bash

export toolchain=intel

module --force purge
ml cluster/dodrio/cpu_rome
ml intel
ml CMake/3.24.3-GCCcore-11.3.0
ml