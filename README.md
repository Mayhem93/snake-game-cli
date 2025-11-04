# Snake Game in CLI

## ⚠️Disclaimer

This project was done for my own pleasure and education as I have not coded in a language like C++ since college. This was made with the help of AI (Copilot with Claude Sonnet)

## Documentation

[Doxygen documentation](https://mayhem93.github.io/snake-game-cli/)

## Prerequisites

* cmake 3.xx
* gcc v13 (if using an older compiler version we might have some issues and I had to upgrade to use some c++ new features)
* vcpkg installed in homedir (if somewhere else, you need to modify `CMakePresets.json`)

## Building

* `cmake --preset linux-make-x64`
* `cmake --build --preset build-linux-make-debug --config debug`

## Running

* `./build/linux-make-x64/snake`

