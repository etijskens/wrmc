#!/bin/bash

if [ "$1" = "" ]
then 
    echo "ERROR: toolchain not provided: [foss|intel]" 1>&2
    return 1
fi

export toolchain=$1
if [ "$toolchain" = "intel" ]; then
    CMake=CMake/3.24.3-GCCcore-11.3.0
elif [ "$toolchain" = "foss" ]; then
    CMake=CMake/3.24.3-GCCcore-12.2.0
else
    echo "Error: Unknown toolchain '$toolchain'" 1>&2
    return 1
fi

module --force purge
ml cluster/dodrio/cpu_rome
ml ${toolchain}
ml ${CMake}
