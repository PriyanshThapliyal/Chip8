#pragma once 
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <cstdint>
#include <SDL2/SDL_render.h>

const int SCALE = 10;
class Display 
{
public:
    bool init();
    void draw(const unsigned char* gfx);
    bool handleInput(unsigned char* key);
    void updateAudio(bool beeping);
    void cleanup();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_AudioDeviceID audioDevice = 0;
    uint32_t pixels[64*32];
};
