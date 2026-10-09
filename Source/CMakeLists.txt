add_library(Chip8_Core)
add_library(Chip8::Core ALIAS Chip8_Core)

find_package(unofficial-sodium CONFIG REQUIRED)

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

target_compile_features(Chip8_Core PUBLIC c_std_23)
target_link_libraries(Chip8_Core PRIVATE Chip8::Warnings unofficial-sodium::sodium)

add_executable(Chip8 Private/Main.c)
target_compile_features(Chip8 PUBLIC c_std_23)
target_link_libraries(Chip8 PRIVATE Chip8::Core Chip8::Warnings)
