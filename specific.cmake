# Try system-installed fmt first (Ubuntu: libfmt-dev, macOS: brew install fmt, Termux: fmt)
find_package(fmt CONFIG QUIET)

if(fmt_FOUND)
  message(STATUS "Found system fmt: ${fmt_DIR}")
  # Tell CPM that fmt is already handled (CPM checks CPM_PACKAGES list). Write the CACHE
  # variable directly: list(APPEND ...) creates a normal-variable shadow that does not
  # propagate into FetchContent subdirectory scopes.
  if(NOT fmt IN_LIST CPM_PACKAGES)
    set(CPM_PACKAGES "${CPM_PACKAGES};fmt" CACHE INTERNAL "" FORCE)
  endif()
else()
  CPMAddPackage(
    NAME fmt
    GIT_TAG 12.1.0
    GITHUB_REPOSITORY fmtlib/fmt
    OPTIONS "FMT_INSTALL YES" # create an installable target
  )
endif()

CPMAddPackage(
  NAME LdsGen
  GIT_TAG v1.2.4
  GITHUB_REPOSITORY luk036/lds-gen-cpp
  OPTIONS "INSTALL_ONLY YES" # create an installable target
)

# CPMAddPackage("gh:xtensor-stack/xtl#0.6.23") if(xtl_ADDED) message(STATUS "Found xtl:
# ${xtl_SOURCE_DIR}") include_directories(${xtl_SOURCE_DIR}/include) endif(xtl_ADDED)
#
# CPMAddPackage("gh:xtensor-stack/xtensor#0.22.0") if(xtensor_ADDED) message(STATUS "Found xtensor:
# ${xtensor_SOURCE_DIR}") include_directories(${xtensor_SOURCE_DIR}/include) endif(xtensor_ADDED)

# set(SPECIFIC_LIBS LdsGen::LdsGen fmt::fmt GSL)
set(SPECIFIC_LIBS LdsGen::LdsGen fmt::fmt)
# remember to turn off the warnings
