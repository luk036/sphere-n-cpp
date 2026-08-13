cmake_minimum_required(VERSION 3.14...3.22)

project(
  Sphere_N
  VERSION 1.1.3
  LANGUAGES CXX
)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---------------------------------------------------------------------------
# Compiler warnings (as errors)
# ---------------------------------------------------------------------------

function(target_enable_warnings TARGET)
  if(MSVC)
    target_compile_options(${TARGET} PRIVATE /W4 /WX /utf-8 /wd4819)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  endif()
endfunction()

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(SPHERE_N_BUILD_TESTS "Build tests" ON)
option(SPHERE_N_BUILD_STANDALONE "Build standalone executable" ON)
option(SPHERE_N_BUILD_DOCS "Build Doxygen documentation" OFF)
option(SPHERE_N_ENABLE_CLANG_TIDY "Enable clang-tidy analysis" OFF)
option(SPHERE_N_ENABLE_COVERAGE "Enable coverage reporting" OFF)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

# Download CPM.cmake for reproducible dependency management
# see https://github.com/cpm-cmake/CPM.cmake for more info
set(CPM_DOWNLOAD_VERSION 0.38.5)
set(CPM_HASH_SUM "192aa0ccdc57dfe75bd9e4b176bf7fb5692fd2b3e3f7b09c74856fc39572b31c")
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
  message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
  file(
    DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
             ${CPM_DOWNLOAD_LOCATION}
    EXPECTED_HASH SHA256=${CPM_HASH_SUM}
  )
endif()
include(${CPM_DOWNLOAD_LOCATION})

# PackageProject.cmake will be used to make our target installable
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.8.0")

# fmt is used for formatting; prefer a system-installed fmt when available.
find_package(fmt CONFIG QUIET)
if(fmt_FOUND)
  message(STATUS "Found system fmt: ${fmt_DIR}")
  set(CPM_fmt_ADDED YES)
else()
  CPMAddPackage(
    NAME fmt
    GIT_TAG 12.1.0
    GITHUB_REPOSITORY fmtlib/fmt
    OPTIONS "FMT_INSTALL YES" # create an installable target
  )
endif()

# LdsGen provides the low-discrepancy sequence generators used across the public headers.
# LdsGen's logger uses spdlog and queries the plain `spdlog` target; building spdlog from source
# first (rather than finding a system one, which only provides spdlog::spdlog) makes that target
# exist and avoids an ALIAS/IMPORTED target collision.
CPMAddPackage(
  NAME spdlog
  GIT_TAG v1.17.0
  GITHUB_REPOSITORY gabime/spdlog
  OPTIONS "SPDLOG_INSTALL YES" "SPDLOG_FMT_EXTERNAL YES"
)

CPMAddPackage(
  NAME LdsGen
  GIT_TAG v1.2.5
  GITHUB_REPOSITORY luk036/lds-gen-cpp
  OPTIONS "LDSGEN_BUILD_TESTS OFF" "LDSGEN_BUILD_STANDALONE OFF"
)

set(SPECIFIC_LIBS LdsGen::LdsGen fmt::fmt)

# ---------------------------------------------------------------------------
# Library target
# ---------------------------------------------------------------------------

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE headers CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
     "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

add_library(${PROJECT_NAME} ${headers} ${sources})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

# Enable interprocedural optimization (LTO) for better inlining and code generation
if(SPHERE_N_ENABLE_COVERAGE)
  # LTO conflicts with gcov instrumentation (undefined __gcov_* symbols at link); disable it.
  set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
else()
  set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

# Public headers include <ldsgen/lds.hpp>, so LdsGen is a public dependency.
target_link_libraries(${PROJECT_NAME} PUBLIC ${SPECIFIC_LIBS})

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

target_enable_warnings(${PROJECT_NAME})

# ---- Create an installable target ----
# this allows users to install and find the library via `find_package()`.

# the location where the project's version header will be placed should match the project's regular
# header paths
string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  NAMESPACE ${PROJECT_NAME}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  VERSION_HEADER "${VERSION_HEADER_LOCATION}"
  COMPATIBILITY SameMajorVersion
  DEPENDENCIES "fmt 12.1.0" "LdsGen 1.2.5"
)

# ---------------------------------------------------------------------------
# Standalone executable
# ---------------------------------------------------------------------------

if(SPHERE_N_BUILD_STANDALONE)
  # cxxopts is a header-only CLI parser used by the standalone example. Prefer a consumer- or
  # system-provided cxxopts; only fall back to CPM when no cxxopts target is available.
  find_package(cxxopts CONFIG QUIET)
  if(NOT cxxopts_FOUND AND NOT TARGET cxxopts::cxxopts)
    CPMAddPackage(
      GITHUB_REPOSITORY jarro2783/cxxopts
      VERSION 3.2.1
      OPTIONS "CXXOPTS_BUILD_EXAMPLES NO" "CXXOPTS_BUILD_TESTS NO" "CXXOPTS_ENABLE_INSTALL NO"
    )
  endif()
  # keep third-party headers out of our -Werror scope
  if(TARGET cxxopts::cxxopts)
    get_target_property(CXXOPTS_INCLUDE_DIRS cxxopts::cxxopts INTERFACE_INCLUDE_DIRECTORIES)
  endif()

  add_executable(Sphere_NStandalone standalone/source/main.cpp)
  set_target_properties(Sphere_NStandalone PROPERTIES OUTPUT_NAME "Sphere_N" CXX_STANDARD 20)
  target_include_directories(Sphere_NStandalone SYSTEM PRIVATE ${CXXOPTS_INCLUDE_DIRS})
  target_link_libraries(Sphere_NStandalone PRIVATE ${PROJECT_NAME}::${PROJECT_NAME}
                                                   cxxopts::cxxopts ${SPECIFIC_LIBS}
  )
  target_enable_warnings(Sphere_NStandalone)
endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

enable_testing()

if(SPHERE_N_BUILD_TESTS)
  # Try system-installed doctest first (Ubuntu: doctest-dev, macOS: brew install doctest, Termux:
  # doctest)
  find_package(doctest CONFIG QUIET)

  if(NOT doctest_FOUND)
    CPMAddPackage(
      NAME doctest
      GITHUB_REPOSITORY doctest/doctest
      VERSION 2.5.2
      OPTIONS "DOCTEST_NO_INSTALL ON" # keep the installed prefix minimal
    )
  endif()

  # Code formatting targets (format / fix-format) via Format.cmake
  CPMAddPackage("gh:TheLartians/Format.cmake@1.7.3")

  file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "test/source/*.cpp")
  add_executable(Sphere_NTests ${TEST_SOURCES})
  target_link_libraries(Sphere_NTests PRIVATE doctest::doctest ${PROJECT_NAME}::${PROJECT_NAME}
                                               ${SPECIFIC_LIBS}
  )
  set_target_properties(Sphere_NTests PROPERTIES CXX_STANDARD 20)
  target_enable_warnings(Sphere_NTests)
  add_test(NAME Sphere_NAllTests COMMAND Sphere_NTests)
endif()

# ---------------------------------------------------------------------------
# clang-tidy
# ---------------------------------------------------------------------------

if(SPHERE_N_ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY clang-tidy)
  if(CLANG_TIDY)
    add_custom_target(
      clang-tidy
      COMMAND ${CLANG_TIDY} --extra-arg=-std=c++20
              --extra-arg=-I${CMAKE_CURRENT_SOURCE_DIR}/include
              ${CMAKE_CURRENT_SOURCE_DIR}/include/sphere_n/*.hpp
      COMMENT "Running clang-tidy static analysis"
    )
  else()
    message(WARNING "clang-tidy not found - analysis target unavailable")
  endif()
endif()

# ---------------------------------------------------------------------------
# Doxygen documentation
# ---------------------------------------------------------------------------

if(SPHERE_N_BUILD_DOCS)
  find_package(Doxygen QUIET)
  if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME ${PROJECT_NAME})
    set(DOXYGEN_PROJECT_VERSION ${PROJECT_VERSION})
    set(DOXYGEN_PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doxygen")

    configure_file(documentation/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    add_custom_target(
      GenerateDocs
      COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
      COMMAND echo "Docs written to: ${DOXYGEN_OUTPUT_DIRECTORY}/html"
      COMMENT "Generating Doxygen HTML documentation in ${DOXYGEN_OUTPUT_DIRECTORY}/html"
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
  else()
    add_custom_target(
      GenerateDocs COMMAND ${CMAKE_COMMAND} -E echo
                           "Doxygen not found - skipping documentation generation"
    )
  endif()
endif()

# ---------------------------------------------------------------------------
# Coverage report target (GCC/Clang only - requires gcovr)
# ---------------------------------------------------------------------------

if(SPHERE_N_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
  # Every target that links the instrumented library also needs the coverage runtime at link time.
  foreach(COVERAGE_TARGET Sphere_N Sphere_NTests Sphere_NStandalone)
    if(TARGET ${COVERAGE_TARGET})
      target_compile_options(${COVERAGE_TARGET} PRIVATE -O0 -g -fprofile-arcs -ftest-coverage)
    endif()
  endforeach()
  foreach(COVERAGE_EXEC Sphere_NTests Sphere_NStandalone)
    if(TARGET ${COVERAGE_EXEC})
      target_link_options(${COVERAGE_EXEC} PRIVATE -fprofile-arcs -ftest-coverage)
    endif()
  endforeach()

  find_program(GCOVR_PATH gcovr)
  if(GCOVR_PATH)
    set(extra_gcovr_args "")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
      find_program(LLVM_COV_PATH llvm-cov)
      if(LLVM_COV_PATH)
        list(APPEND extra_gcovr_args --llvm-cov-binary ${LLVM_COV_PATH})
      endif()
    endif()
    add_custom_target(
      coverage
      COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
      COMMAND ${GCOVR_PATH} --root ${PROJECT_SOURCE_DIR} --html-details
              ${CMAKE_BINARY_DIR}/coverage/index.html --exclude test/ --exclude build/
              ${extra_gcovr_args}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating coverage report in ${CMAKE_BINARY_DIR}/coverage/index.html"
    )
  else()
    message(WARNING "gcovr not found - coverage target unavailable. Install with: pip install gcovr")
  endif()
endif()
