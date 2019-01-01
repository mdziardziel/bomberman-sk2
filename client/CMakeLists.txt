# via https://cmake.org/cmake-tutorial/
cmake_minimum_required (VERSION 2.6)
project (BombermanClient)

# The version number.
set (BombermanClient_VERSION_MAJOR 1)
set (BombermanClient_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/ClientConfig.hpp.in"
  "${PROJECT_BINARY_DIR}/ClientConfig.hpp"
  )

# add the binary tree to the search path for include files
# so that we will find ClientConfig.hpp
include_directories("${PROJECT_BINARY_DIR}")

add_executable(bin/BombermanClient main.cpp)