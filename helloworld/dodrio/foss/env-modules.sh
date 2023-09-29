#!/bin/bash

module --force purge
ml cluster/dodrio/cpu_rome
ml foss
ml CMake/3.24.3-GCCcore-12.2.0
ml