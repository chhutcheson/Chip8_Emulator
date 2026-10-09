#include <Chip8/Chip8.h>

#include <stdio.h>

int main(void) {
    static Chip8 chip;
    Chip8_Init(&chip);

    printf("CHIP-8 ready, PC = 0x%03X\n", (unsigned)chip.PC);
}
