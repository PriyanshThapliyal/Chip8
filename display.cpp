#include <SDL2/SDL_audio.h>
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

const Palette palettes[] = {
    { "Classic Monochrome", 0xFF121212, 0xFFE0E0E0},
    { "Phosphor Green"    , 0xFF081408, 0xFF33FF66},
    { "Amber CRT"         , 0xFF140A02, 0xFFFF8000},
    { "Game Boy 1989"     , 0xFF1F381F, 0xFF8BAC0F},
    { "Nord Frost"        , 0xFF242933, 0xFF88C0D0}
};

const int NUM_PALETTES = sizeof(palettes) / sizeof(palettes[0]);

void Display::cycleTheme()      // Method to change Theme
{
    currentTheme = (currentTheme + 1) % NUM_PALETTES;
    std::cout << "Theme : " << palettes[currentTheme].name << std::endl;
}


static void audioCallback(void* userdata, Uint8* stream, int len)
{
    (void)userdata;
    int16_t* buffer = reinterpret_cast<int16_t*>(stream);
    int samples = len / sizeof(int16_t);
    static int runningSampleIndex = 0;
    const int sampleRate = 44100;
    const int toneHz = 440;
    const int halfPeriod = sampleRate / toneHz / 2;

    for (int i = 0; i < samples; ++i)
    {
        buffer[i] = ((runningSampleIndex / halfPeriod) % 2 == 0) ? 3000 : -3000;
        runningSampleIndex++;
    }
}

bool Display::init()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
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
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 128, 64);

    if(!texture)
    {
        std::cout << "Texture Creation failed : " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 2048;
    want.callback = audioCallback;

    audioDevice = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if(audioDevice == 0)
    {
        std::cout << "Warning: Failed to open audio device: " << SDL_GetError() << std::endl;
    }

    return true;
}


void Display::draw(const unsigned char* gfx, bool isHighRes)
{
    uint32_t fg = palettes[currentTheme].fg;
    uint32_t bg = palettes[currentTheme].bg;

    if (isHighRes)
    {
        for(int i = 0; i < 128*64; i++)
        {
            pixels[i] = gfx[i] ? fg : bg;
        }
    }
    else
    {
        for(int y = 0; y < 32; y++)
        {
            for(int x = 0; x < 64; x++)
            {
                uint32_t color = gfx[x + (y * 64)] ? fg : bg;
                int pX = x * 2;
                int pY = y * 2;
                pixels[pX + (pY * 128)] = color;
                pixels[(pX + 1) + (pY * 128)] = color;
                pixels[pX + ((pY + 1) * 128)] = color;
                pixels[(pX + 1) + ((pY + 1) * 128)] = color;
            }
        }
    }

    SDL_UpdateTexture(texture, nullptr, pixels, 128*sizeof(uint32_t));

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Display::updateAudio(bool beeping)
{
    if(audioDevice != 0)
    {
        SDL_PauseAudioDevice(audioDevice, beeping ? 0 : 1);
    }
}

bool Display::handleInput(unsigned char* key, int& speed, bool& paused, bool& reset)
{
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        switch(e.type)
        {
            case SDL_QUIT: return false;
            case SDL_KEYDOWN:if(e.key.keysym.sym == SDLK_ESCAPE)
                                return false;
                             
                             if(e.key.keysym.sym == SDLK_EQUALS)
                                speed += 5 , std::cout << "Cycle / Frame : " << speed << std::endl;
                             else if(e.key.keysym.sym == SDLK_MINUS)
                                speed = speed > 5 ? speed - 5 : 1, std::cout << " Cycle / Frame : "   << speed << std::endl;
                
                             if(e.key.keysym.sym == SDLK_k)
                                cycleTheme();
                            
                             if(e.key.keysym.sym == SDLK_p)
                                paused = paused ^ true;
                             if(e.key.keysym.sym == SDLK_o)
                                reset = true;

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
    if(audioDevice != 0)
    {
        SDL_CloseAudioDevice(audioDevice);
        audioDevice = 0;
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


