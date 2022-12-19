cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(Cala)

find_package(OpenGL 4.3 REQUIRED)

set(dependencies_dir ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies)

add_subdirectory(Dependencies)
add_subdirectory(src)