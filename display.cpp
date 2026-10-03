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
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

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

void Display::updateAudio(bool beeping)
{
    if(audioDevice != 0)
    {
        SDL_PauseAudioDevice(audioDevice, beeping ? 0 : 1);
    }
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


