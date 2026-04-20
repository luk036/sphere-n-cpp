# AGENTS.md - Developer Guide for sphere-n-cpp

> Configuration guide for agentic coding agents working in this repository.

## Project Overview

- **Name**: SphereN (sphere-n-cpp)
- **Language**: C++20 (C++17 minimum)
- **Build System**: CMake 3.14+ with CPM.cmake
- **Alternative Build**: xmake.lua available
- **Test Framework**: doctest
- **Purpose**: Low discrepancy sequence generation for n-dimensional spheres

---

## Build Commands

### Full Build (CMake)
```bash
cmake -S . -B build
cmake --build build
```

### Running Tests
```bash
# Via ctest
cd build/test && ctest --build-config Debug

# Or run executable directly
./build/test/SphereNTests

# Run specific test case
./build/test/SphereNTests -tc="Sphere3"
./build/test/SphereNTests -tc="SphereN"
./build/test/SphereNTests -tc="CylindN"

# List available test cases
./build/test/SphereNTests -ltc
```

### Code Formatting
```bash
cmake -S . -B build
cmake --build build --target format    # Check
cmake --build build --target fix-format  # Apply
```

### Static Analysis
```bash
cmake -S . -B build -DUSE_STATIC_ANALYZER=clang-tidy
cmake -S . -B build -DUSE_STATIC_ANALYZER=iwyu
cmake -S . -B build -DUSE_STATIC_ANALYZER=cppcheck
```

### Build with Sanitizers
```bash
cmake -S . -B build -DUSE_SANITIZER=Address
cmake -S . -B build -DUSE_SANITIZER=Undefined
cmake -S . -B build -DUSE_SANITIZER="Address;Undefined"
```

### xmake (Alternative)
```bash
xmake              # Build
xmake test         # Run tests
xmake run test_sphere_n
```

---

## Code Style Guidelines

### Formatting (.clang-format)

| Setting | Value |
|---------|-------|
| BasedOnStyle | Google |
| ColumnLimit | 100 |
| IndentWidth | 4 |
| AccessModifierOffset | -2 |
| NamespaceIndentation | All |
| BreakBeforeBraces | Attach |
| IncludeBlocks | Regroup |

### Includes Order

1. Primary header for this source
2. System headers (`<vector>`, `<array>`, `<span>`, etc.)
3. External libraries (`<ldsgen/lds.hpp>`, `<fmt/format.h>`)
4. Project headers (`<sphere_n/sphere_n.hpp>`)

### Naming Conventions

| Element | Convention | Example |
|---------|-----------|---------|
| Classes | PascalCase | `Sphere3`, `SphereN` |
| Functions | snake_case | `pop()`, `reseed()` |
| Variables | snake_case | `vdc`, `sphere2` |
| Constants | PascalCase or SCREAMING_SNAKE | `N_POINTS`, `PI` |
| Member variables | snake_case with `_` | `vdc_`, `n_` |
| Files | snake_case | `sphere_n.hpp` |
| Namespaces | snake_case | `lds2`, `ldsgen` |

### Type Usage

- Use STL: `std::vector`, `std::array`, `std::span`, `std::unique_ptr`
- Use `std::variant` for type-safe unions
- Prefer `std::span<const T>` over raw pointers
- Use `size_t` for sizes/indices
- Use `auto` for type deduction

### Error Handling

- Use `assert()` for invariants
- Document preconditions in function docs

### Class Layout
```cpp
class Foo {
  public:
    Foo();
    explicit Foo(span<const size_t> base);
    auto pop() -> return_type;
    auto reseed(size_t seed) -> void;
  private:
    VdCorput vdc_;
    size_t n_;
};
```

---

## Project Structure

```
sphere-n-cpp/
├── include/sphere_n/     # Public headers
│   ├── sphere_n.hpp
│   └── cylind_n.hpp
├── source/              # Implementation
│   ├── sphere_n.cpp
│   └── cylind_n.cpp
├── test/source/         # Tests
│   ├── main.cpp
│   └── test_lds_n.cpp
├── cmake/              # CMake modules
├── lds-gen-cpp/       # External sibling dependency
├── CMakeLists.txt
├── xmake.lua
└── .clang-format
```

---

## External Dependencies

- **lds-gen-cpp**: Required sibling at `../lds-gen-cpp`
  - Include: `#include <ldsgen/lds.hpp>`
  - Provides: `VdCorput`, `Sphere` classes
- **doctest**: Testing (via CPM)
- **fmt**: Formatting (via CPM)

---

## CI/CD

- `.github/workflows/windows.yml` - Windows MSVC
- `.github/workflows/ubuntu.yml` - Ubuntu with coverage
- `.github/workflows/macos.yml` - macOS
- `CTEST_OUTPUT_ON_FAILURE=1`

---

## Adding New Tests

In `test/source/test_lds_n.cpp`:
```cpp
TEST_CASE("Descriptive Name") {
    // doctest CHECK_* macros
}
```

---

## Adding New Source/Header

- Add to `source/` or `include/sphere_n/`
- CMake glob picks up automatically
- Rebuild to include