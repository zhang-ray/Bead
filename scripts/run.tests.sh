#!/bin/bash
# copy resources
cp ./tests/UiGenerator/app00.xml /tmp/


../build/simulator/Simulator
../build/tests/UiGenerator/UiGeneratorTest
