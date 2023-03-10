CPMAddPackage(
  NAME fmt
  GIT_TAG 7.1.3
  GITHUB_REPOSITORY fmtlib/fmt
  OPTIONS "FMT_INSTALL YES" # create an installable target
)

CPMAddPackage("gh:microsoft/GSL@3.1.0")

CPMAddPackage(
  NAME Lds
  GIT_TAG 1.0.5
  GITHUB_REPOSITORY luk036/lds-cpp
  OPTIONS "INSTALL_ONLY YES" # create an installable target
)

CPMAddPackage("gh:xtensor-stack/xtl#0.6.23")
if(xtl_ADDED)
  message(STATUS "Found xtl: ${xtl_SOURCE_DIR}")
  include_directories(${xtl_SOURCE_DIR}/include)
endif(xtl_ADDED)

CPMAddPackage("gh:xtensor-stack/xtensor#0.22.0")
if(xtensor_ADDED)
  message(STATUS "Found xtensor: ${xtensor_SOURCE_DIR}")
  include_directories(${xtensor_SOURCE_DIR}/include)
endif(xtensor_ADDED)

set(SPECIFIC_LIBS fmt::fmt GSL Lds::Lds)
# remember to turn off the warnings
