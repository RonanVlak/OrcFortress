#!/bin/bash

# Clean previous build
if [ -d Code/build ]; then
    rm -rf Code/build
fi

mkdir Code/build

cd Code/build || exit 1

cmake .. -DCMAKE_BUILD_TYPE=Debug
if [ $? -ne 0 ]; then
    echo "CMake configuration failed."
    exit 1
fi

make -j16
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi

cd ../.. || exit 1

if [ -f session_log.txt ]; then
    rm session_log.txt
fi
./Code/build/bin/OrcFortress