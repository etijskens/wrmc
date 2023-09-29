#!/bin/bash
#!/bin/bash

if [ "$1" = "" ]
then 
    echo "ERROR: toolchain not provided: [foss|intel]" 1>&2
    return 1
fi

export toolchain=$1

module --force purge
ml calcua/2022a
ml $toolchain
ml CMake
ml