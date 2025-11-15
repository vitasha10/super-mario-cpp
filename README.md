# Super Mario C++

A simple platformer game written in C++ using Windows Console API.

## Building

### Windows with MinGW/Git Bash
```bash
./build.sh
```

### Windows Command Prompt
```cmd
build.bat
```

### Manual Build
```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -o super-mario.exe main.cpp
```

## Controls

- **SPACE** - Jump
- **A** - Move left (scroll map right)
- **D** - Move right (scroll map left)
- **ESC** - Exit game

## Requirements

- Windows OS
- C++17 compatible compiler (g++ recommended)
- No external libraries required

## Game Features

- 3 levels with different layouts
- Enemies that patrol platforms
- Question blocks that spawn coins
- Score tracking
- Level completion system
