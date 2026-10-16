#include <Chip8/Chip8.h>
#include <Platform/Platform.h>

#include <glib.h>

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <Scale> <Delay> <ROM>\n", argv[0]);
        exit(1);
    }

    int videoScale = atoi(argv[1]);
    int cycleDelay = atoi(argv[2]);
    char const *romFilename = argv[3];

    Platform platform;
    Platform_Init(&platform, "CHIP-8 Emulator",
                    (int32_t)CHIP8_FRAME_BUFFER_W * videoScale,
                    (int32_t)CHIP8_FRAME_BUFFER_H * videoScale,
                    CHIP8_FRAME_BUFFER_W, CHIP8_FRAME_BUFFER_H);

    Chip8 chip;
    Chip8_LoadRom(&chip, romFilename);

    int videoPitch = sizeof(chip.FrameBuffer[0]) * CHIP8_FRAME_BUFFER_W;
    
    bool quit = false;

    while (!quit){
        quit = Platform_ProcessInput(chip.Keypad);
        GTimer* timer = g_timer_new();

        if (1000*g_timer_elapsed(timer, nullptr) > (float)cycleDelay)
        {
            Chip8_Cycle(&chip);

            Platform_Update(&platform, chip.FrameBuffer, videoPitch);
        }
    }
}
