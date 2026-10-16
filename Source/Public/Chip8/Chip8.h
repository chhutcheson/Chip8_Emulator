#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CHIP8_MEMORY_SIZE         4096
#define CHIP8_MEMORY_MASK         0xfffu
#define CHIP8_PROGRAM_START       0x200
#define CHIP8_FRAME_BUFFER_W      64
#define CHIP8_FRAME_BUFFER_H      32
#define CHIP8_FRAME_BUFFER_SIZE   CHIP8_FRAME_BUFFER_W * CHIP8_FRAME_BUFFER_H
#define  CHIP8_OP_WIDTH           2

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
