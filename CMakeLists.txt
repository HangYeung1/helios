cmake_minimum_required(VERSION 3.16)

project(helios LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(helios ${SOURCES})
