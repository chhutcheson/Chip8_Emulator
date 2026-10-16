#include <Platform/Platform.h>

#include <SDL3/SDL.h>
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

#include <stdint.h>
#include <stdio.h>

void Platform_Init(Platform *p, const char *title, int32_t wndWidth, 
    int32_t wndHeight, int32_t txtWidth, int32_t txtHeight) {
    SDL_Init(SDL_INIT_VIDEO);
    
    p->window = SDL_CreateWindow(title, wndWidth, wndHeight, 
        SDL_WINDOW_RESIZABLE);
    if (!p->window) {
        char const*err = SDL_GetError();
        fprintf(stderr, "Error: %s", err);
    }

    p->renderer = SDL_CreateRenderer(p->window, NULL);
    if (!p->renderer) {
        char const*err = SDL_GetError();
        fprintf(stderr, "Error: %s", err);
    }

    p->texture = SDL_CreateTexture(p->renderer, SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_STREAMING, txtWidth, txtHeight);
    if (!p->texture) {
        char const*err = SDL_GetError();
        fprintf(stderr, "Error: %s", err);
    }

    SDL_ShowWindow(p->window);
}

void Platform_Update(Platform *p, void const* frameBuffer, int pitch) {
    SDL_UpdateTexture(p->texture, NULL, frameBuffer, pitch);
    SDL_RenderClear(p->renderer);
    SDL_RenderTexture(p->renderer, p->texture, NULL, NULL);
    SDL_RenderPresent(p->renderer);
}

void Platform_Shutdown(Platform *p) {
    // destroy in reverse order of creation, NULL is safe for each
    SDL_DestroyTexture(p->texture);
    SDL_DestroyRenderer(p->renderer);
    SDL_DestroyWindow(p->window);
    SDL_Quit();
}

bool Platform_ProcessInput(uint8_t* keys) {
    bool quit = false;

    SDL_Event event;

    while (SDL_PollEvent(&event))
{
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
            {
                quit = true;
            } break;

            case SDL_EVENT_KEY_DOWN:
            {
                switch (event.key.key)
                {
                    case SDLK_ESCAPE:
                    {
                        quit = true;
                    } break;

                    case SDLK_X:
                    {
                        keys[0] = 1;
                    } break;

                    case SDLK_1:
                    {
                        keys[1] = 1;
                    } break;

                    case SDLK_2:
                    {
                        keys[2] = 1;
                    } break;

                    case SDLK_3:
                    {
                        keys[3] = 1;
                    } break;

                    case SDLK_Q:
                    {
                        keys[4] = 1;
                    } break;

                    case SDLK_W:
                    {
                        keys[5] = 1;
                    } break;

                    case SDLK_E:
                    {
                        keys[6] = 1;
                    } break;

                    case SDLK_A:
                    {
                        keys[7] = 1;
                    } break;

                    case SDLK_S:
                    {
                        keys[8] = 1;
                    } break;

                    case SDLK_D:
                    {
                        keys[9] = 1;
                    } break;

                    case SDLK_Z:
                    {
                        keys[0xA] = 1;
                    } break;

                    case SDLK_C:
                    {
                        keys[0xB] = 1;
                    } break;

                    case SDLK_4:
                    {
                        keys[0xC] = 1;
                    } break;

                    case SDLK_R:
                    {
                        keys[0xD] = 1;
                    } break;

                    case SDLK_F:
                    {
                        keys[0xE] = 1;
                    } break;

                    case SDLK_V:
                    {
                        keys[0xF] = 1;
                    } break;
                }
            } break;

            case SDL_EVENT_KEY_UP:
            {
                switch (event.key.key)
                {
                    case SDLK_X:
                    {
                        keys[0] = 0;
                    } break;

                    case SDLK_1:
                    {
                        keys[1] = 0;
                    } break;

                    case SDLK_2:
                    {
                        keys[2] = 0;
                    } break;

                    case SDLK_3:
                    {
                        keys[3] = 0;
                    } break;

                    case SDLK_Q:
                    {
                        keys[4] = 0;
                    } break;

                    case SDLK_W:
                    {
                        keys[5] = 0;
                    } break;

                    case SDLK_E:
                    {
                        keys[6] = 0;
                    } break;

                    case SDLK_A:
                    {
                        keys[7] = 0;
                    } break;

                    case SDLK_S:
                    {
                        keys[8] = 0;
                    } break;

                    case SDLK_D:
                    {
                        keys[9] = 0;
                    } break;

                    case SDLK_Z:
                    {
                        keys[0xA] = 0;
                    } break;

                    case SDLK_C:
                    {
                        keys[0xB] = 0;
                    } break;

                    case SDLK_4:
                    {
                        keys[0xC] = 0;
                    } break;

                    case SDLK_R:
                    {
                        keys[0xD] = 0;
                    } break;

                    case SDLK_F:
                    {
                        keys[0xE] = 0;
                    } break;

                    case SDLK_V:
                    {
                        keys[0xF] = 0;
                    } break;
                }
            } break;
        }
    }

    return quit;
}