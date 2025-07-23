# Tic Tac Toe Project

## Dev setup

You need to generate files for the LSP to know about SDL:

```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 vendored/SDL
```

## Build

```bash
cmake -S . -B build
cmake --build build
```
