cmake_minimum_required (VERSION 2.6)
project (BombermanServer)

# The version number.
set (BombermanServer_VERSION_MAJOR 1)
set (BombermanServer_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/ServerConfig.hpp.in"
  "${PROJECT_BINARY_DIR}/ServerConfig.hpp"
  )

# add the binary tree to the search path for include files
# so that we will find ServerConfig.h
include_directories("${PROJECT_BINARY_DIR}")

add_executable(BombermanServer main.cpp)