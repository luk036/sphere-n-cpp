# Project Overview

This is a C++ library for generating low-discrepancy sequences on n-dimensional spheres. Low-discrepancy sequences are used to generate points that are distributed uniformly across a given space, which is useful in fields like computer graphics, numerical integration, and Monte Carlo simulations.

The library is written in C++20 and uses modern CMake for its build system. It has dependencies on `fmt` for string formatting, `doctest` for unit testing, and `lds-gen-cpp`, another library by the same author.

# Building and Running

The project uses CMake for building. Here are the commands to build and run the various targets:

## Build and run the standalone target

```bash
cmake -S. -B build
cmake --build build
./build/standalone/SphereN --help
```

## Build and run test suite

```bash
cmake -S. -B build
cmake --build build
cd build/test
CTEST_OUTPUT_ON_FAILURE=1 ctest
```

## Run clang-format

```bash
cmake -S . -B build/test
# view changes
cmake --build build --target format
# apply changes
cmake --build build --target fix-format
```

## Build the documentation

```bash
cmake -S . -B build
cmake --build build --target GenerateDocs
# view the docs
open build/documentation/doxygen/html/index.html
```

An alternative build system using `xmake` is also available.

# Development Conventions

*   **Coding Style:** The project follows the Google C++ Style Guide with some modifications. The style is enforced by `.clang-format`.
*   **Testing:** Unit tests are written using the `doctest` framework. Test files are located in the `test/source` directory.
*   **Continuous Integration:** The project uses GitHub Actions to build and test the code on Ubuntu. The workflow is defined in `.github/workflows/ubuntu.yml`.
*   **Documentation:** Documentation is generated using Doxygen. The configuration is in `documentation/Doxyfile`.
