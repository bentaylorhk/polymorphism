# Benjamin Michael Taylor (bentaylorhk)
# 2025

cmake_minimum_required(VERSION 3.31)

project(
    Polymorphism
    VERSION 1.0
    DESCRIPTION "Collection of ascii art animations for Polyphonic"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_COMPILER_WARNING_AS_ERROR ON)

find_package(CLI11 REQUIRED)

add_compile_definitions(ASCII_ART_DIR="${CMAKE_SOURCE_DIR}/ascii-art/")

add_subdirectory(src)
