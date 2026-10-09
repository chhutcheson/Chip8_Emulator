cmake_minimum_required(VERSION 3.25)

option(CHIP8_BUILD_TESTS "Build the GoogleTest unit tests" ON)

# Must be set before project() so the vcpkg toolchain installs the test deps.
if(CHIP8_BUILD_TESTS)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

project(chip8
    VERSION 0.1.0
    DESCRIPTION "CHIP-8 emulator"
    LANGUAGES C
)

set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# Shared warning flags, linked privately into every first-party target.
add_library(Chip8_Warnings INTERFACE)
add_library(Chip8::Warnings ALIAS Chip8_Warnings)

if(MSVC)
    target_compile_options(Chip8_Warnings INTERFACE /W4 /permissive-)
else()
    target_compile_options(Chip8_Warnings INTERFACE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow
    )
endif()

add_subdirectory(Source)

if(CHIP8_BUILD_TESTS)
    enable_language(CXX)
    enable_testing()
    add_subdirectory(Tests)
endif()
