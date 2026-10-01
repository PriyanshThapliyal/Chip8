#include "chip8.hpp"

void Chip8::initialize()
{
    pc = 0x200; // Program counter starts at 0x200
    opcode = 0; // Reset current opcode
    I = 0;      // Reset index register
    sp = 0;     // Reset stack pointer


    for(int i = 0; i < 80; ++i)
    {
        memory[i] = chip8_fontset[i];
    }
}

void Chip8::emulateCycle()
{
    // Encoding Opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Decoding opcode
    switch(opcode & 0xF000)
    {
        case 0xA000: I = opcode & 0x0FFF;    // ANNN : Sets I to the address of NNN
                     pc += 2;
                     break;

        case 0x0000: switch(opcode & 0x000F)
                     {
                        case 0x0000:        // 0x00E0 : clears screen
                                    break;
                        case 0x000E:        // 0x00EE : returns from subroutines
                                    break;
                        default: std::cout << "Uknown opcode [0x0000] : " << opcode;
                     }
                     break;

        case 0x2000:stack[sp] = pc; // 0x2NNN : Calls subroutines at NNN
                    ++sp;           // Temporarily need to jump at NNN so store current address in stack
                    pc = opcode & 0x0FFF;
                    break;
        
        case 0x0004:if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) // Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.
                    V[0xF] = 1; // Carry
                    else 
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;


        default:
            std::cout << "Uknown opcode" << opcode;
    }

    if(delayTimer > 0)
        --delayTimer;

    if(soundTimer > 0)
    {
        if(soundTimer == 0)
            std::cout << "BEEP!\n";
        --soundTimer;
    }
}
