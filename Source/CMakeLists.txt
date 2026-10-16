add_library(Chip8_Core)
add_library(Chip8::Core ALIAS Chip8_Core)

add_library(Chip8_Platform)
add_library(Chip8::Platform ALIAS Chip8_Platform)

find_package(unofficial-sodium CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)

target_sources(Chip8_Core
    PRIVATE
        Private/Chip8.c
        Private/Helpers.c
        Private/Helpers.h
        Private/Font.h
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS Public
        FILES Public/Chip8/Chip8.h

)

target_sources(Chip8_Platform
    PRIVATE
        Private/Platform.c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS Public
        FILES Public/Platform/Platform.h
)

target_compile_features(Chip8_Core PUBLIC c_std_23)
target_link_libraries(Chip8_Core PRIVATE Chip8::Warnings 
                                            unofficial-sodium::sodium)


target_compile_features(Chip8_Platform PUBLIC c_std_23)
target_link_libraries(Chip8_Platform PRIVATE SDL3::SDL3)

add_executable(Chip8 Private/Main.c)
target_compile_features(Chip8 PUBLIC c_std_23)
target_link_libraries(Chip8 PRIVATE Chip8::Core Chip8::Platform Chip8::Warnings PkgConfig::GLIB)
