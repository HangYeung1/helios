cmake_minimum_required(VERSION 3.16)
project(helios LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glm REQUIRED)

add_executable(helios src/main.cpp src/Camera.cpp)
target_compile_options(helios PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(helios PRIVATE glm::glm)