@echo off
REM Build script for Super Mario C++ game
REM Requires MinGW g++ compiler with C++17 support

echo Building Super Mario C++...

g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -o super-mario.exe main.cpp

if %errorlevel% equ 0 (
    echo Build successful! Executable: super-mario.exe
) else (
    echo Build failed!
    exit /b 1
)
