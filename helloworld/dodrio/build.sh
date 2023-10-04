#!/bin/bash
# execute this script from its parent directory.

. ./env-modules.sh $1
if [ "$?" = "1" ]
then
    echo "Aborting" 1>&2
    exit 1
fi

mkdir -p ${toolchain}/_build
cd ${toolchain}/_build
cmake ../../..
cmake --build .
cmake --install . --prefix ../