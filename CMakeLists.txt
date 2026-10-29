cmake_minimum_required(VERSION 3.16)

project(ViSil LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Visil_Includes INTERFACE)
target_include_directories(Visil_Includes INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_subdirectory(src/core)
add_subdirectory(src/arch)
add_subdirectory(src/devices)
add_subdirectory(src/cli)
add_subdirectory(src/gui)

enable_testing()
add_subdirectory(tests)
