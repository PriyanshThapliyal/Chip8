#pragma once
#include <iostream>

unsigned char chip8_fontset[] = {
                                0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                                0x20, 0x60, 0x20, 0x20, 0x70, // 1 
                                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2 
                                0xF0, 0x20, 0x40, 0x20, 0xF0, // 3
                                0x20, 0x60, 0xF0, 0x20, 0x20, // 4 
                                0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                                0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6 
                                0xF0, 0x10, 0x20, 0x40, 0x40, // 7 
                                0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8 
                                0xF0, 0x90, 0x10, 0x10, 0xF0, // 9 
                                0xF0, 0x90, 0xF0, 0x90, 0x90, // A 
                                0xE0, 0x90, 0xE0, 0x90, 0xE0, // B 
                                0xF0, 0x80, 0x80, 0x80, 0xF0, // C 
                                0xE0, 0x90, 0x90, 0x90, 0xE0, // D 
                                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E 
                                0xF0, 0x80, 0xF0, 0x80, 0x80, // F 
                              };
class Chip8
{
    public:
        void initialize();
        void emulateCycle();

private:
    unsigned short opcode;

    // Chip 8 have 4Kb(Kibibytes) of memory
    unsigned char memory[4096];

    // Chip 8 have 16 8-bit registers
    unsigned char V[16];

    unsigned short I; // Index register
    unsigned short pc; // Program counter 

    // 2048 pixels n Chip 8 
    unsigned char gfx[64*32];

    unsigned char delayTimer;
    unsigned char soundTimer; 

    unsigned short stack[16];
    unsigned short sp;

    unsigned char key[16]; // hex based keypad
};
