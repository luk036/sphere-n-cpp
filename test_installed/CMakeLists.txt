cmake_minimum_required(VERSION 3.14)

project(Sphere_NInstalledTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find dependencies first - Sphere_NConfig.cmake references fmt and LdsGen (installed alongside the
# library by `cmake --install`).
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(LdsGen REQUIRED)
find_package(Sphere_N REQUIRED)

add_executable(sphere_n_installed_test main.cpp)
target_link_libraries(sphere_n_installed_test PRIVATE Sphere_N::Sphere_N)

enable_testing()
add_test(NAME sphere_n_installed_test COMMAND sphere_n_installed_test)
