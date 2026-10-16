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
    Chip8_Init(&chip);
    if (Chip8_LoadRom(&chip, romFilename) != 0) {
        fprintf(stderr, "Error: unable to load ROM %s\n", romFilename);
        Platform_Shutdown(&platform);
        return 1;
    }

    int videoPitch = sizeof(chip.FrameBuffer[0]) * CHIP8_FRAME_BUFFER_W;

    bool quit = false;
    GTimer* timer = g_timer_new();

    while (!quit){
        quit = Platform_ProcessInput(chip.Keypad);

        if (1000*g_timer_elapsed(timer, NULL) > (float)cycleDelay)
        {
            g_timer_start(timer); // restart the delay for the next cycle

            Chip8_Cycle(&chip);

            Platform_Update(&platform, chip.FrameBuffer, videoPitch);
        }
    }

    g_timer_destroy(timer);
    Platform_Shutdown(&platform);
    return 0;
}
