#!/bin/bash

. env-modules.sh

mkdir -p _build
cd _build
cmake ../../..
cmake --build .
cmake --install . --prefix ../