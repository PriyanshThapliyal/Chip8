#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <cstdint>
#include "display.hpp"

const SDL_Keycode keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v
};

bool Display::init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL failed to init : "  << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64*SCALE, 32*SCALE, SDL_WINDOW_SHOWN);

    if(!window)
    {
        std::cout << "Window Creation Failed : " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if(!renderer)
    {
        std::cout << "renderer creation failed : " << SDL_GetError() << std::endl;
        return false;
    }
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    if(!texture)
    {
        std::cout << "Texture Creation failed : " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}


void Display::draw(const unsigned char* gfx)
{
    for(int i = 0; i < 64*32; i++)
    {
        pixels[i] = gfx[i] ? 0xFFFFFFFF
                            :0xFF000000;
    }

    SDL_UpdateTexture(texture, nullptr, pixels, 64*sizeof(uint32_t));

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Display::handleInput(unsigned char* key)
{
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_QUIT: return false;
            case SDL_KEYDOWN:if(e.key.keysym.sym == SDLK_ESCAPE)
                                return false;
                             for(int i = 0; i < 16; i++)
                             {
                                if(e.key.keysym.sym == keymap[i])
                                    key[i] = 1;
                             }
                             break;
            
            case SDL_KEYUP:for(int i = 0; i < 16; i++)
                            {
                                if(e.key.keysym.sym == keymap[i])
                                    key[i] = 0;
                            }
                            break;
        }
    }
    return true;
}

void Display::cleanup()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


