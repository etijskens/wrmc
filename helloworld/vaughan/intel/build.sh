#!/bin/bash

. env.sh

mkdir -p _build
cd _build
cmake ../../..
cmake --build .
cmake --install . --prefix ../