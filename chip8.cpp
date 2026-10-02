#include "chip8.hpp"
#include <cstring>
#include <fstream>

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



void Chip8::initialize()
{
    std::srand(std::time(nullptr));
    pc = 0x200; // Program counter starts at 0x200
    opcode = 0; // Reset current opcode
    I = 0;      // Reset index register
    sp = 0;     // Reset stack pointer

    memset(memory, 0, sizeof(memory));
    memset(gfx, 0, sizeof(gfx));
    memset(V, 0, sizeof(V));
    memset(stack, 0, sizeof(stack));
    memset(key, 0, sizeof(key));

    for(int i = 0; i < 80; ++i)
    {
        memory[i] = chip8_fontset[i];
    }
}

Chip8::Chip8()
{

}

Chip8::~Chip8()
{

}

void Chip8::emulateCycle()
{
    // Encoding Opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Decoding opcode
    switch(opcode & 0xF000)
    {
        case 0x0000: switch(opcode & 0x00FF)
                     {
                        case 0x00E0:memset(gfx, 0, sizeof(gfx));        // 0x00E0 : clears screen
                                    drawFlag = true;
                                    pc += 2;
                                    break;

                        case 0x00EE:--sp;       // 0x00EE : returns from subroutines
                                    pc = stack[sp];
                                    pc += 2;
                                    break;

                        default: std::cout << "Uknown opcode [0x0000] : " << opcode;
                     }
                     break;

        case 0x1000: pc = opcode & 0x0FFF;
                    break;

        case 0x2000:stack[sp] = pc; // 0x2NNN : Calls subroutines at NNN
                    ++sp;           // Temporarily need to jump at NNN so store current address in stack
                    pc = opcode & 0x0FFF;
                    break;

        case 0x3000:if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF )) // 0x3XKK : skips next instruction if Vx = KK 
                        pc += 4;
                    else
                        pc += 2; 
                    break;

        case 0x4000:if(V[(opcode & 0x0F00) >> 8] != (opcode &  0x00FF))  // 0x4XKK : skips next instruction if Vx != KK 
                        pc += 4;
                    else
                        pc += 2;
                    break;

        case 0x5000:if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) //  0x5XY0 : skips next instruction if Vx == Vy
                        pc += 4;
                    else
                        pc += 2;
                    break;

        case 0x6000:V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);  // 0x6XKK : puts value of Vx = KK
                    pc += 2;
                    break;

        case 0x7000:V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF); // 0x7XKK : increment Vx by KK
                    pc += 2;
                    break;

        case 0x8000:
            switch(opcode & 0x000F)
            {
                case 0x0000:V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4]; // 0x8XY0 : puts value of Vy in Vx
                            pc += 2;
                            break;

                case 0x0001:V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4]; // 0x8XY1 : performs bitwise OR Vx = Vx | Vy
                            pc += 2;
                            break;

                case 0x0002:V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0X00F0) >> 4]; // 0x8XY2 : performs bitwise AND Vx = Vx & Vy
                            pc += 2;
                            break;

                case 0x0003:V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4]; // 0x8XY3 : performs XOR Vx = Vx ^ Vy
                            pc += 2;
                            break;

                case 0x0004:if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) // 0x8XY4 : Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not.
                                V[0xF] = 1; // Carry
                            else 
                                V[0xF] = 0;
                            pc += 2;
                            V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                            break;

                case 0x0005:if(V[(opcode & 0x0F00) >> 8] >= V[(opcode & 0x00F0) >> 4])       // 0x8XY5 : if Vx > Vy then VF = 1 else Vf = 0, Vx = Vx - Vy
                                V[0xF] = 1;
                            else
                                V[0xF] = 0;
                            V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                            pc += 2;
                            break;

                case 0x0006:if(V[(opcode & 0x0F00) >> 8] & 1)       // 0x8XY6 : if LSB if Vx is 1, then VF is set to 1 else 0 , Vx is divided by 2
                                V[0xF] = 1;
                            else
                                V[0xF] = 0;
                            V[(opcode & 0x0F00) >> 8] >>= 1;
                            pc += 2;
                            break;

                case 0x0007:if(V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4])       // 0x8XY7 : if Vy > Vx then VF = 1 else Vf = 0 , Vx = Vy - Vx
                                V[0xF] = 1;
                            else 
                                V[0xF] = 0;
                            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                            pc += 2;
                            break;

                case 0x000E:if(V[(opcode & 0x0F00) >> 8] & 0x80)       // 0x8XYE : if MSB of Vx is 1 , then VF is set to 1 else 0 , Vx is multiplied by 2
                                V[0xF] = 1;
                            else 
                                V[0xF] = 0;
                            V[(opcode & 0x0F00) >> 8] <<= 1;
                            pc += 2;
                            break;

                default: std::cout << "Unknown opcode  [0x8000] :" << opcode ;
            }
            break;
            
        case 0x9000:if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])      // 0x9XY0 : Skips the instruction if Vx != Vy
                        pc += 4;
                    else
                        pc += 2;
                    break;

        case 0xA000: I = opcode & 0x0FFF;   // 0xANNN : Sets register I = NNN
                    pc += 2;
                    break;

        case 0xB000: pc = (opcode & 0x0FFF) + V[0];     //0xBNNN : jumps to the address of NNN + V0
                    break;
            
        case 0xC000:V[(opcode & 0x0F00) >> 8] = (std::rand() % 256) & (opcode & 0x00FF);
                    pc += 2;
                    break;

        case 0xD000:        // 0xDXYN : Vx , Vy stores position of x and y coord and n stores the height 
        {
            unsigned short xCoord = V[(opcode & 0x0F00) >> 8];
            unsigned short yCoord = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;     // Reset collision flag
            
            for(int y = 0; y < height; y++)
            {
                pixel = memory[I + y];      // Get one row of sprite from memory

                for(int x = 0; x < 8; x++)
                {
                    if((pixel & (0x80 >> x)) != 0)
                    {
                        int screenX = (xCoord + x) % 64;
                        int screenY = (yCoord + y) % 32;
                        int pos = screenX + (screenY * 64);

                        // Collision : pixel was already one
                        if(gfx[pos] == 1)
                            V[0xF] = 1;

                        // XOR to pixel
                        gfx[pos] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc += 2;
                break;
        }
        
        case 0xE000: 
                    switch(opcode & 0x00FF)
                    {
                        case 0x009E:if(key[V[(opcode & 0x0F00) >> 8]] != 0)         // 0xEX9E : skips if key Vx is pressed
                                        pc += 4;
                                    else 
                                        pc += 2;
                                    break;
                        
                        case 0x00A1:if(key[V[(opcode & 0x0F00) >> 8]] == 0)         // 0xEXA1 : skips if key Vx is not pressed
                                        pc += 4;
                                    else 
                                        pc += 2;
                                    break;
                    }
                    break;

        case 0xF000:switch(opcode & 0x00FF)
                    {
                        case 0x0007: V[(opcode & 0x0F00) >> 8] = delayTimer;        // 0xFX07 : sets Vx = delay Timer
                                    pc += 2;
                                    break;
                        
                        case 0x000A:
                        {
                                    bool keyPressed = false;        // 0xFX0A : waits for key press, store it in Vx
                                    for(int i = 0; i < 16; i++)
                                    {
                                        if(key[i] != 0)
                                        {
                                            V[(opcode & 0x0F00) >> 8] = i;
                                            keyPressed = true;
                                        }
                                    }
                                    if(!keyPressed)
                                        return;
                                    pc += 2;
                                    break;
                        }

                        case 0x0015: delayTimer = V[(opcode & 0x0F00) >> 8];        // 0xFX15 : Set Delay Timer = Vx
                                    pc += 2;
                                    break;

                        case 0x0018: soundTimer = V[(opcode & 0x0F00) >> 8];        // 0xFX18 : Set Sound Timer = Vx
                                    pc += 2;
                                    break;

                        case 0x001E: I = I + V[(opcode & 0x0F00) >> 8];         // 0xFX1E : I = I + Vx
                                    pc += 2;
                                    break;

                        case 0x0029: I = V[(opcode & 0x0F00) >> 8] * 5;         // I = location of sprite for digit Vx
                                    pc += 2;
                                    break;

                        case 0x0033:memory[I] = V[(opcode & 0x0F00) >> 8] / 100;        // 0xFX33 : Store BCD of Vx in memory starting form I
                                    memory[I+1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                                    memory[I+2] = V[(opcode & 0x0F00) >> 8] % 10;
                                    pc += 2;
                                    break;

                        case 0x0055:for(int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)     // 0xFX55 : Store register V0 - VX at location starting form I
                                    {
                                       memory[I+i] = V[i]; 
                                    }
                                    pc += 2;
                                    break;

                        case 0x0065:for(int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)     // 0xFX65 : Read register V0 - VX from memory starting at location I
                                    {
                                        V[i] = memory[I+i];
                                    }
                                    pc += 2;
                                    break;
                    }
                    break;

            
        
        default:
            std::cout << "Uknown opcode" << opcode;
    }

    if(delayTimer > 0)
        --delayTimer;

    if(soundTimer > 0)
    {
        if(soundTimer == 1)
            std::cout << "BEEP!\n";
        --soundTimer;
    }
}

bool Chip8::loadRom(const char * filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);     // Opening file in binary opcode, ate : at the end

    if(!file.is_open())
    {
        std::cout << "Failed to load ROM : " << filename << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();        //File size
    file.seekg(0, std::ios::beg);

    // Check if ROM fits in memory (4096 - 512 = 3584 B)
    if(size > (4098 - 512))
    {
        std::cout << "ROM is too large : " << size << std::endl;
        return false;
    }

    // reading rom into memory 0x200 
    file.read(reinterpret_cast<char*>(&memory[0x200]), size);
    file.close();

    std::cout << "Loaded ROM  : " << filename << std::endl;
    return true;
}
