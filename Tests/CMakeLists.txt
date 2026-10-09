find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(Chip8_Tests Chip8_Tests.cpp)

target_compile_features(Chip8_Tests PRIVATE cxx_std_20)
target_link_libraries(Chip8_Tests
    PRIVATE
        Chip8::Core
        Chip8::Warnings
        GTest::gtest_main
)

gtest_discover_tests(Chip8_Tests DISCOVERY_MODE PRE_TEST)
