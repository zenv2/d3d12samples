# Top-level CMakeLists.txt

cmake_minimum_required(VERSION 3.5)
project(d3d12samples)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Subdirectories
add_subdirectory(render)
add_subdirectory(apps)
