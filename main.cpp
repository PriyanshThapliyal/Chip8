#include <SDL2/SDL_timer.h>
#include <iostream>
#include "chip8.hpp"
#include "display.hpp"

int cyclePerFrame = 5;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cout << "Usage : " << argv[0] << " <ROM> [speed] \n";
        return 1;
    }
    if(argc == 3)       // Takes the speed as optional argument and changes emulateCycle
    {
        cyclePerFrame = std::stoi(argv[2]);
    }
    
    Chip8 chip8;
    chip8.initialize();

    if(!chip8.loadRom(argv[1]))
        return 1;

    Display display;
    if(!display.init())
        return 1;

    uint32_t lastTimerTime = SDL_GetTicks();

    while(display.handleInput(chip8.key, cyclePerFrame))
    {
        for(int i = 0; i < cyclePerFrame; i++)
        {
            chip8.emulateCycle();
        }

        uint32_t currentTime = SDL_GetTicks();
        if(currentTime - lastTimerTime >= 16) // ~60 Hz (every 16.6ms)
        {
            chip8.updateTimers();
            lastTimerTime = currentTime;
        }

        display.updateAudio(chip8.soundTimer > 0);

        if(chip8.drawFlag)
        {
            display.draw(chip8.gfx);
            chip8.drawFlag = false;
        }

        SDL_Delay(2);
    }
    display.cleanup();
    return 0;
}
