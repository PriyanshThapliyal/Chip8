#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>

class Chip8
{
    public:
        Chip8();
        ~Chip8();

        void initialize();
        void emulateCycle();
        void updateTimers();
        bool loadRom(const char * filename);
        bool drawFlag = false;
        unsigned char key[16]; // hex based keypad
        // 2048 pixels n Chip 8 
        unsigned char gfx[128*64];
        bool isHighRes = false;
        unsigned char delayTimer = 0;
        unsigned char soundTimer = 0;

private:
    unsigned short opcode;

    // Chip 8 have 4Kb(Kibibytes) of memory
    unsigned char memory[4096];

    // Chip 8 have 16 8-bit registers
    unsigned char V[16];

    unsigned short I; // Index register
    unsigned short pc; // Program counter 

    unsigned short stack[16];
    unsigned short sp;

};
