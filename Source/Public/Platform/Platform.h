#ifndef CHIP8_PLATFORM_H
#define CHIP8_PLATFORM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    struct SDL_Window* window;
    struct SDL_Renderer* renderer;
    struct SDL_Texture* texture;
} Platform;

void Platform_Init(Platform *p, const char *title, int32_t wndWidth, int32_t wndHeight, int32_t txtWidth, int32_t txtHeight);
bool Platform_ProcessInput(uint8_t* keys);
void Platform_Update(Platform *p, void const* frameBuffer, int pitch);
void Platform_Shutdown(Platform *p);
#endif // CHIP8_PLATFORM_H
