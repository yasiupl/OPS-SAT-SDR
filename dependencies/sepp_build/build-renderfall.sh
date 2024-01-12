#!/usr/bin/env bash

cd renderfall
mkdir -p build
cd build
cmake ..
make 
make install