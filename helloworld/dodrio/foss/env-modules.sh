#!/bin/bash

export toolchain=$1

module --force purge
ml cluster/dodrio/cpu_rome
ml $toolchain
ml CMake/3.24.3-GCCcore-12.2.0
ml