cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(Cala)

find_package(OpenGL 4.3 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Dependencies)
add_subdirectory(Cala)
add_subdirectory(Calion)