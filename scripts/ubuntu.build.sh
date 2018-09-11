#!/bin/bash
sudo apt --quiet -y install libsdl2-dev libsdl2-ttf-dev  # for simulator

mkdir ../build
cd ../build
cmake ../bead
make -j3

