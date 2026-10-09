#include <Chip8/Chip8.h>

#include <gtest/gtest.h>

TEST(Chip8Init, SetsProgramCounterToProgramStart) {
    Chip8 chip{};
    chip.PC = 0xFFFF;

    Chip8_Init(&chip);

    EXPECT_EQ(chip.PC, CHIP8_PROGRAM_START);
}
