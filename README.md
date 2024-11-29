[![Actions Status](https://github.com/luk036/sphere-n-cpp/workflows/MacOS/badge.svg)](https://github.com/luk036/sphere-n-cpp/actions)
[![Actions Status](https://github.com/luk036/sphere-n-cpp/workflows/Windows/badge.svg)](https://github.com/luk036/sphere-n-cpp/actions)
[![Actions Status](https://github.com/luk036/sphere-n-cpp/workflows/Ubuntu/badge.svg)](https://github.com/luk036/sphere-n-cpp/actions)
[![Actions Status](https://github.com/luk036/sphere-n-cpp/workflows/Style/badge.svg)](https://github.com/luk036/sphere-n-cpp/actions)
[![Actions Status](https://github.com/luk036/sphere-n-cpp/workflows/Install/badge.svg)](https://github.com/luk036/sphere-n-cpp/actions)
[![codecov](https://codecov.io/gh/luk036/sphere-n-cpp/graph/badge.svg?token=u0caEVx7c9)](https://codecov.io/gh/luk036/sphere-n-cpp)

<p align="center">
  <img src="https://repository-images.githubusercontent.com/254842585/4dfa7580-7ffb-11ea-99d0-46b8fe2f4170" height="175" width="auto" />
</p>

# ⚽ sphere-n-cpp

> Low discrepancy sequence for modern C++

This library implements a generator for the generation of low-discrepancy sequences on n-dimensional spheres. Low-discrepancy sequences are employed to generate points that are distributed uniformly across a given space. This is a valuable technique in a number of fields, including computer graphics, numerical integration, and Monte Carlo simulations.

The primary objective of this library is to facilitate the generation of points on the surface of spheres of varying dimensions, including three-dimensional and higher-dimensional spheres. The input required is the dimension of the sphere (n) and a set of base numbers to be used for the underlying sequence generation. The output is a series of vectors, with each vector representing a point on the surface of the n-dimensional sphere.

The library achieves this through a combination of mathematical calculations and recursive structures. The library employs a number of fundamental components, including:

1. The VdCorput sequence generator produces evenly distributed numbers between 0 and 1.
2. The aforementioned numerical data is then mapped onto the surface of a sphere through the use of interpolation functions.
3. The recursive structures, designated as Sphere3 and NSphere, facilitate the construction of higher-dimensional entities from their lower-dimensional counterparts.

The primary logic flow commences with the construction of a SphereN object, which employs either a Sphere3 (for three-dimensional applications) or a recursive process to generate lower-dimensional spheres for higher dimensions. In the generation of points, the VdCorput sequence is employed to obtain a fundamental number, which is then subjected to a series of transformations involving the sine, cosine, and interpolation functions, thereby mapping it onto the surface of the sphere.

Furthermore, the library furnishes traits and structures that facilitate the adaptable utilisation of the sphere generators. The SphereGen trait establishes a common interface for disparate sphere generators, whereas the NSphere and SphereN structures implement the actual generation logic.

In conclusion, this library offers a sophisticated yet flexible approach to the generation of evenly distributed points on high-dimensional spheres, which can be advantageous in a multitude of scientific and computational applications.

Requirements:

- This library requires C++17 or above.
- It uses [CPM.cmake](https://github.com/TheLartians/CPM.cmake) for dependency management.
- It uses [fmt](https://github.com/fmtlib/fmt) for formatting.
- It uses [doctest](https://github.com/onqtam/doctest) for unit testing.

## ✨ Features

- Support N-Sphere
- [Modern CMake practices](https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/)
- Integrated test suite
- Continuous integration via [GitHub Actions](https://help.github.com/en/actions/)
- Code coverage via [codecov](https://codecov.io)
- Code formatting enforced by [clang-format](https://clang.llvm.org/docs/ClangFormat.html) and [cmake-format](https://github.com/cheshirekow/cmake_format) via [Format.cmake](https://github.com/TheLartians/Format.cmake)
- Reproducible dependency management via [CPM.cmake](https://github.com/TheLartians/CPM.cmake)
- Installable target with automatic versioning information and header generation via [PackageProject.cmake](https://github.com/TheLartians/PackageProject.cmake)
- Automatic [documentation](https://thelartians.github.io/ModernCppStarter) and deployment with [Doxygen](https://www.doxygen.nl) and [GitHub Pages](https://pages.github.com)
- Support for [sanitizer tools, and more](#additional-tools)

## Usage

### Adjust the template to your needs

- Use this repo [as a template](https://help.github.com/en/github/creating-cloning-and-archiving-repositories/creating-a-repository-from-a-template).
- Replace all occurrences of "SphereN" in the relevant CMakeLists.txt with the name of your project
  - Capitalization matters here: `SphereN` means the name of the project, while `sphere_n` is used in file names.
  - Remember to rename the `include/sphere_n` directory to use your project's lowercase name and update all relevant `#include`s accordingly.
- Replace the source files with your own
- For header-only libraries: see the comments in [CMakeLists.txt](CMakeLists.txt)
- Add [your project's codecov token](https://docs.codecov.io/docs/quick-start) to your project's github secrets under `CODECOV_TOKEN`
- Happy coding!

Eventually, you can remove any unused files, such as the standalone directory or irrelevant github workflows for your project.
Feel free to replace the License with one suited for your project.

~~To cleanly separate the library and subproject code, the outer `CMakeList.txt` only defines the library itself while the tests and other subprojects are self-contained in their own directories.
During development it is usually convenient to [build all subprojects at once](#build-everything-at-once).~~

### Build and run the standalone target

Use the following command to build and run the executable target.

```bash
cmake -S. -B build
cmake --build build
./build/standalone/SphereN --help
```

### Build and run test suite

Use the following commands from the project's root directory to run the test suite.

```bash
cmake -S. -B build
cmake --build build
cd build/test
CTEST_OUTPUT_ON_FAILURE=1 ctest

# or maybe simply call the executable:
./build/test/SphereNTests
```

To collect code coverage information, run CMake with the `-DENABLE_TEST_COVERAGE=1` option.

### Run clang-format

Use the following commands from the project's root directory to check and fix C++ and CMake source style.
This requires _clang-format_, _cmake-format_ and _pyyaml_ to be installed on the current system.

```bash
cmake -S . -B build/test

# view changes
cmake --build build --target format

# apply changes
cmake --build build --target fix-format
```

See [Format.cmake](https://github.com/TheLartians/Format.cmake) for details.

### Build the documentation

The documentation is automatically built and [published](https://luk036.github.io/sphere-n-cpp) whenever a [GitHub Release](https://help.github.com/en/github/administering-a-repository/managing-releases-in-a-repository) is created.
To manually build documentation, call the following command.

```bash
cmake -S . -B build
cmake --build build --target GenerateDocs
# view the docs
open build/documentation/doxygen/html/index.html
```

To build the documentation locally, you will need Doxygen, jinja2 and Pygments on installed your system.

### Additional tools

The test and standalone subprojects include the [tools.cmake](cmake/tools.cmake) file which is used to import additional tools on-demand through CMake configuration arguments.
The following are currently supported.

#### Sanitizers

Sanitizers can be enabled by configuring CMake with `-DUSE_SANITIZER=<Address | Memory | MemoryWithOrigins | Undefined | Thread | Leak | 'Address;Undefined'>`.

#### Static Analyzers

Static Analyzers can be enabled by setting `-DUSE_STATIC_ANALYZER=<clang-tidy | iwyu | cppcheck>`, or a combination of those in quotation marks, separated by semicolons.
By default, analyzers will automatically find configuration files such as `.clang-format`.
Additional arguments can be passed to the analyzers by setting the `CLANG_TIDY_ARGS`, `IWYU_ARGS` or `CPPCHECK_ARGS` variables.

#### Ccache

Ccache can be enabled by configuring with `-DUSE_CCACHE=<ON | OFF>`.

## Related projects and alternatives

- [**ModernCppStarter & PVS-Studio Static Code Analyzer**](https://github.com/viva64/pvs-studio-cmake-examples/tree/master/modern-cpp-starter): Official instructions on how to use the ModernCppStarter with the PVS-Studio Static Code Analyzer.
- [**lefticus/cpp_starter_project**](https://github.com/lefticus/cpp_starter_project/): A popular C++ starter project, created in 2017.
- [**filipdutescu/modern-cpp-template**](https://github.com/filipdutescu/modern-cpp-template): A recent starter using a more traditional approach for CMake structure and dependency management.
- [**vector-of-bool/pitchfork**](https://github.com/vector-of-bool/pitchfork/): Pitchfork is a Set of C++ Project Conventions.
