# via https://cmake.org/cmake-tutorial/
cmake_minimum_required (VERSION 2.6)
project (BombermanServer)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall")

# The version number.
set (BombermanServer_VERSION_MAJOR 1)
set (BombermanServer_VERSION_MINOR 1)
set (CMAKE_CXX_STANDARD 11)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/ServerConfig.hpp.in"
  "${PROJECT_BINARY_DIR}/ServerConfig.hpp"
  )

# add the binary tree to the search path for include files
# so that we will find ServerConfig.hpp
include_directories("${PROJECT_BINARY_DIR}")

include_directories ("${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
add_subdirectory (lib) 
 
# add executable 
add_executable(BombermanServer main.cpp)
target_link_libraries(BombermanServer Threads::Threads)
target_link_libraries (BombermanServer Helpers)
target_link_libraries (BombermanServer Engine)
target_link_libraries (BombermanServer Server)