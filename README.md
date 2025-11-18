# OrcFortress

This repository contains the source, headers and build files for the OrcFortress game.
This is the final assignment for the course Advanced CPP programming of the Systems Programming in CPP Minor at Avans Hogeschool

## Short description

OrcFortress is a C++ project (CMake) that builds a small game executable. The source is organized into frontend and backend components and includes an embedded sqlite3 external dependency.
The frontend uses items from the STL library (like std::vector and std::string) and only smart pointers.
The backend does not use the STL library, and only uses raw pointers and dynamic arrays.

## Requirements

- A C++ toolchain (g++ or clang++)
- CMake 3.10+ (or the version your distro provides)
- make

## Build

From the repository root:

```bash
mkdir -p build
cd build
cmake ..
make -j 8
```

If the build completes successfully the executable is placed in `build/bin/OrcFortress`.

## Run

Run the built executable directly:

```bash
./build/bin/OrcFortress
```

There are also helper scripts:

- `run.sh` — run the game (removes the build directory and rebuilds the project from scratch)
- `run_valgrind.sh` — run the game under Valgrind for memory checks (rebuilds the project)

## Project layout

- `inc/` — public headers
- `src/` — implementation files
- `Code/` — CMake and generated build artifacts (this includes the `build/` tree)
- `external/` — bundled third-party code (sqlite3)
- `Resources/` — contains the database used in this project
