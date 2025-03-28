CPMAddPackage(
  NAME fmt
  GIT_TAG 10.2.1
  GITHUB_REPOSITORY fmtlib/fmt
  OPTIONS "FMT_INSTALL YES" # create an installable target
)

CPMAddPackage(
  NAME LdsGen
  GIT_TAG 1.0.4
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
