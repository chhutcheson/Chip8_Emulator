#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static constexpr uint16_t CHIP8_MEMORY_SIZE         = 4096;
static constexpr uint16_t CHIP8_MEMORY_MASK         = 0xfffu; // 4096 - 1
static constexpr uint16_t CHIP8_PROGRAM_START       = 0x200;
//                                                    w  * l * u32 size
static constexpr uint32_t CHIP8_FRAME_BUFFER_W      = 64;
static constexpr uint32_t CHIP8_FRAME_BUFFER_H      = 32;
static constexpr uint32_t CHIP8_FRAME_BUFFER_SIZE   = CHIP8_FRAME_BUFFER_W * CHIP8_FRAME_BUFFER_H;
static constexpr uint8_t  CHIP8_OP_WIDTH            = 2; // ops all 2 bytes wide

struct Chip8;
typedef void (*Chip8OpImpl)(struct Chip8*);

typedef struct Chip8 {
    uint8_t Registers[16];
    uint8_t Memory[CHIP8_MEMORY_SIZE];
    uint16_t Index;
    uint16_t PC;
    uint16_t Stack[16];
    uint16_t SP;
    uint8_t DelayTimer;
    uint8_t SoundTimer;
    uint8_t Keypad[16];
    uint32_t FrameBuffer[CHIP8_FRAME_BUFFER_SIZE];
    uint16_t OpCode;
    Chip8OpImpl OpTable[16];
} Chip8;

void Chip8_Init(Chip8 *chip);
int Chip8_LoadRom(Chip8 *chip, char const *filename);
void Chip8_Cycle(Chip8 *chip);

#ifdef __cplusplus
}
#endif

#endif // CHIP8_CHIP8_H
