#include <SDL2/SDL_timer.h>
#include <iostream>
#include "chip8.hpp"
#include "display.hpp"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage : " << argv[0] << std::endl;
        return 1;
    }

    Chip8 chip8;
    chip8.initialize();

    if(!chip8.loadRom(argv[1]))
        return 1;

    Display display;
    if(!display.init())
        return 1;

    while(display.handleInput(chip8.key))
    {
        chip8.emulateCycle();

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
