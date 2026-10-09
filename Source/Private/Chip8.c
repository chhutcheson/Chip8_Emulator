#include <Chip8/Chip8.h>
#include "Font.h"
#include "Helpers.h"

#include <sodium/randombytes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sodium.h>

void Chip8_Init(Chip8 *chip) {
    // zero init struct
    memset(chip, 0, sizeof(*chip));

    chip->PC = CHIP8_PROGRAM_START;

    // load font
    memcpy(&chip->Memory[CHIP8_FONT_START], CHIP8_FONTSET, sizeof(CHIP8_FONTSET));

    // load RNG
    if (sodium_init() < 0) {
        fprintf(stderr, "Error: unable to init libsodium\n");
        exit(69);
    }
}

/**
 * Change the ROM loaded in to memory. If there was a problem reading the file
 * this function returns -60. Returns -69 if the ROM is too big for the memory
 */
int Chip8_LoadRom(Chip8 *chip, char const *filename) {
    Optional_FatString maybeFileContents = ReadFileToString(filename);
    if (!maybeFileContents.Ok) {
        return -60; // signal error reading rom to str
    }

    FatString fileContents = maybeFileContents.Value;
    
    // ROM is too big for the memory, how??
    if (fileContents.Length > CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_START) {
        return -69; // too big for memory!!
    }

    // load rom into memory
    memcpy(&chip->Memory[CHIP8_PROGRAM_START], fileContents.String,
        fileContents.Length);
    free(fileContents.String);
    return 0;
}

static uint8_t Chip8_RandByte() {
    // sodium takes care of the rand gen, just take from it, exclusice range
    return (uint8_t)randombytes_uniform(256);
}

#pragma region Operations

/**
 * Clears the screen
 */
static void Chip8_Op_00E0(Chip8 *chip) {
  memset(chip->FrameBuffer, 0, sizeof(chip->FrameBuffer));
}

/**
 * Return from a subroutine
 */
static void Chip8_Op_00EE(Chip8 *chip) {
    // go one instruction past the one that called subroutine
    chip->PC = chip->Stack[--chip->SP]; // top of stack holds the ret location
}

/**
 * Jump to nnn, where nnn is the OpCode currently being processed
 */
static void Chip8_Op_1nnn(Chip8 *chip) {
    // program counter goes straight to OpCode,
    // addr to jmp to in last 3 hx pos of op
    chip->PC = chip->OpCode & CHIP8_MEMORY_MASK;
}

/** 
 * Call subroutine
 */
static void Chip8_Op_2nnn(Chip8 *chip) {
    chip->Stack[chip->SP++] = chip->PC; // store return location
    Chip8_Op_1nnn(chip); // jmp to subroutine
}

/**
 * Skip next operation if register x holds value kk
 */
static void Chip8_Op_3xkk(Chip8 *chip) {
    uint8_t reg = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    uint8_t byte = chip->OpCode & 0x00ffu; // get first byte

    // only increment if reg contains byte
    chip->PC += (chip->Registers[reg] == byte) * CHIP8_OP_WIDTH;
}

/**
 * Skip next operation if register x does not hold value kk
 */
static void Chip8_Op_4xkk(Chip8 *chip) {
    uint8_t reg = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    uint8_t byte = chip->OpCode & 0x00ffu; // get first byte

    // only increment if reg not contains byte
    chip->PC += (chip->Registers[reg] != byte) * CHIP8_OP_WIDTH;
}

/**
 * Skip next operation if register x == register y
 */
static void Chip8_Op_5xy0(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    uint8_t y = (chip->OpCode & 0x00f0u) >> 4u; // get 2nd nybble

    // only increment if reg contains byte
    chip->PC += (chip->Registers[x] == chip->Registers[y]) * CHIP8_OP_WIDTH;
}

/**
 * Load kk into register x
 */
static void Chip8_Op_6xkk(Chip8 *chip) {
    uint8_t reg = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    uint8_t byte = chip->OpCode & 0x00ffu; // get first byte

    chip->Registers[reg] = byte; // load into reg
}

/**
 * Add value in register x with int kk
 */
static void Chip8_Op_7xkk(Chip8 *chip) {
    uint8_t reg = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    uint8_t byte = chip->OpCode & 0x00ffu; // get first byte

    chip->Registers[reg] += byte;
}

/**
 * Load register y into register x
 */
static void Chip8_Op_8xy0(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00) >> 8u; // get 3rd nybble
    uint8_t y = (chip->OpCode & 0x00f0) >> 4u; // get 2nd nybble

    chip->Registers[x] = chip->Registers[y];
}
/**
 * Register x | register y
 */
static void Chip8_Op_8xy1(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00) >> 8u; // get 3rd nybble
    uint8_t y = (chip->OpCode & 0x00f0) >> 4u; // get 2nd nybble
    
    chip->Registers[x] |= chip->Registers[y];
}

/**
 * Register x & register y
 */
static void Chip8_Op_8xy2(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00) >> 8u; // get 3rd nybble
    uint8_t y = (chip->OpCode & 0x00f0) >> 4u; // get 2nd nybble
    
    chip->Registers[x] &= chip->Registers[y];
}

/**
 * Register x ^ register y
 */
static void Chip8_Op_8xy3(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00) >> 8u; // get 3rd nybble
    uint8_t y = (chip->OpCode & 0x00f0) >> 4u; // get 2nd nybble
    
    chip->Registers[x] ^= chip->Registers[y];
}

/**
 * Add with carry register y into register x
 */
static void Chip8_Op_8xy4(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00) >> 8u; // get 3rd nybble
    uint8_t y = (chip->OpCode & 0x00f0) >> 4u; // get 2nd nybble
    
    uint16_t sum = chip->Registers[x] + chip->Registers[y];
    if (sum > 0xffu) { // overflown the byte
        chip->Registers[0xf] = 1; // overflow flag set
    } else {
        chip->Registers[0xf] = 0; // reset overflow flag
    }

    chip->Registers[x] = sum & 0xff;
}

/**
 * Sub with carry register y into register x
 */
static void Chip8_Op_8xy5(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00) >> 8u; // get 3rd nybble
    uint8_t y = (chip->OpCode & 0x00f0) >> 4u; // get 2nd nybble
    
    if (chip->Registers[x] > chip->Registers[y]) { // underflown the byte
        chip->Registers[0xf] = 1; // underflown flag set
    } else {
        chip->Registers[0xf] = 0; // reset underflown flag
    }
    chip->Registers[x] -= chip->Registers[y];
}

/**
 * Right shift (SHR) register x > 1
 */
static void Chip8_Op_8xy6(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00) >> 8u; // get 3rd nybble

    // least sig bit gets stored in register F
    chip->Registers[0xf] = chip->Registers[x] & 0x1;
    chip->Registers[x] >>= 1;
}

/**
 * Sub register y from register x with no borrow (subn)
 */
static void Chip8_Op_8xy7(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00) >> 8u; // get 3rd nybble
    uint8_t y = (chip->OpCode & 0x00f0) >> 4u; // get 2nd nybble

    if (chip->Registers[x] < chip->Registers[y]) { // DO NOT BORROW
        chip->Registers[0xf] = 1; // no borrow flag set
    } else {
        chip->Registers[0xf] = 0; // reset no borrow flag
    }
    chip->Registers[x] = chip->Registers[y] - chip->Registers[x];
}

/**
 * Right shift (SHL) register x < 1
 */
static void Chip8_Op_8xyE(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00) >> 8u; // get 3rd nybble

    // most sig bit gets stored in register F
    chip->Registers[0xf] = (chip->Registers[x] & 0x80) >> 7u;
    chip->Registers[x] <<= 1;
}

/**
 * Skip next op if register x != register y
 */
static void Chip8_Op_9xy0(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    uint8_t y = (chip->OpCode & 0x00f0u) >> 4u; // get 2nd nybble

    // only increment if reg not contains byte
    chip->PC += (chip->Registers[x] != chip->Registers[y]) * CHIP8_OP_WIDTH;
}

/**
 * Load nnn into chip->Index
 */
static void Chip8_Op_Annn(Chip8 *chip) {
    chip->Index = chip->OpCode & CHIP8_MEMORY_MASK;
}

/**
 *  Jump to location nnn + register 0
 */
static void Chip8_Op_Bnnn(Chip8 *chip) {
    // program counter goes straight to OpCode,
    // addr to jmp to in last 3 hx pos of op
    chip->PC = (chip->OpCode & CHIP8_MEMORY_MASK) + chip->Registers[0];
}

/**
 *  Set register x to random byte & kk
 */
static void Chip8_Op_Cxkk(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    uint8_t byte = chip->OpCode & 0x00ffu; // get first byte

    chip->Registers[x] = Chip8_RandByte() & byte;
}

/**
 * Display n-byte sprite starting at Index -> coords(reg x, reg y) and set reg F
 * for collision
 */
static void Chip8_Op_Dxyn(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00) >> 8u; // get 3rd nybble
    uint8_t y = (chip->OpCode & 0x00f0) >> 4u; // get 2nd nybble
    uint8_t h = chip->OpCode & 0x000fu; // get 1st nybble

    // wrap the screen if we start overscanning
    uint8_t xPos = chip->Registers[x] % CHIP8_FRAME_BUFFER_W;
    uint8_t yPos = chip->Registers[y] % CHIP8_FRAME_BUFFER_H;

    chip->Registers[0xf] = 0; // reset flag to get new condition

    for (uint32_t row = 0; row < h; ++row) {
        uint8_t spriteByte = chip->Memory[chip->Index + row];
        for (uint32_t col = 0; col < 8; ++col) {
            uint8_t spritePx = spriteByte & (0x80u >> col); // each bit is a px
            uint32_t *screenPx = &chip->FrameBuffer[(yPos + row) * CHIP8_FRAME_BUFFER_W // go down y+r rows
                                                    +(xPos + col)]; // col offset
            // sprite pixel is on ie colored
            if (spritePx) {
                // screen pixel is already lit ... collision
                if (*screenPx == 0xffff'ffff) {
                    chip->Registers[0xf] = 1;
                }

                // toggle the screen pixel 
                *screenPx ^= 0xffff'ffff;
            }
        }
    }
}

/**
 * Skip next op if a key is pressed
 */
static void Chip8_Op_Ex9E(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    uint8_t key = chip->Registers[x];

    chip->PC += (chip->Keypad[key] != 0) * CHIP8_OP_WIDTH;
}

/**
 * Skip next op if a key is not pressed
 */
static void Chip8_Op_ExA1(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    uint8_t key = chip->Registers[x];

    chip->PC += (chip->Keypad[key] == 0) * CHIP8_OP_WIDTH;
}

/**
 * Load the delay timer value into register x
 */
static void  Chip8_Op_Fx07(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble

    chip->Registers[x] = chip->DelayTimer;
}

/**
 * Wait for a key press and store it in to register x
 */
static void  Chip8_Op_Fx0A(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble

    chip->Registers[x] = chip->DelayTimer;

    // keypad is arr of u8, so sizeof == num elems
    bool isKeyPressed = false;
    for (uint8_t i = 0; i < sizeof(chip->Keypad); ++i) {
        if (chip->Keypad[i]) { // key pressed?
            chip->Registers[x] = i;
            isKeyPressed = true;
            break;
        }
    }

    if (!isKeyPressed) {
        // rerun this instruction again until we get a key press
        chip->PC -= CHIP8_OP_WIDTH;
    }
}

/**
 * Set the delay timer with the value in register x
 */
static void  Chip8_Op_Fx15(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    chip->DelayTimer = chip->Registers[x];
}

/**
 * Set the sound timer with the value in register x
 */
static void  Chip8_Op_Fx18(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    chip->SoundTimer = chip->Registers[x];
}

/**
 * Add the value in register x to the chip Index
 */
static void  Chip8_Op_Fx1E(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    chip->Index += chip->Registers[x];
}

/**
 * Set the Index to the address of the sprite for value in register x
 */
static void  Chip8_Op_Fx29(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    uint8_t digit = chip->Registers[x];
    chip->Index = CHIP8_FONT_START + (5 * digit);
}

/**
 * Load the binary coded decimal representation of the value in register x into
 * the next 3 bytes from Index into memory
 */
static void  Chip8_Op_Fx33(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble
    uint8_t value = chip->Registers[x];

    // ones place
    chip->Memory[chip->Index + 2]   = value % 10;
    // tens place
    chip->Memory[chip->Index + 1]   = value % 10;
    // hundreds place
    chip->Memory[chip->Index]       = value % 10;
}

/**
 * Stores registers [0,x] into addresses starting from Index
 */
static void  Chip8_Op_Fx55(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble

    for (uint8_t i = 0; i <= x; ++i) {
        chip->Memory[chip->Index + i] = chip->Registers[i];
    }
}

/**
 * Load starting from Index into registers [0,x]
 */
static void  Chip8_Op_Fx65(Chip8 *chip) {
    uint8_t x = (chip->OpCode & 0x0f00u) >> 8u; // get 3rd nybble

    for (uint8_t i = 0; i <= x; ++i) {
        chip->Registers[i] = chip->Memory[chip->Index + i];
    }
}

#pragma endregion