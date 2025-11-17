#!/bin/bash
# Build script for Super Mario C++ game
# Requires g++ compiler with C++17 support

set -e

echo "Building Super Mario C++..."

g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic \
    -o super-mario.exe \
    main.cpp

echo "Build successful! Executable: super-mario.exe"
echo "Note: This game requires Windows to run."
