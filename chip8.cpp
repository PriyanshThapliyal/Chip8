#include "chip8.hpp"
#include <cstring>
#include <fstream>
#include <cstdint>

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
    isHighRes = false;

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

                        case 0x00C0 ... 0x00CF:        // 0x00CN : Scroll down by N pixels 
                        {
                            int n = opcode & 0x000F;
                            int w = isHighRes ? 128 : 64;
                            int h = isHighRes ? 64 : 32;
                            for(int y = h - 1; y >= n; --y)
                            {
                                for(int x = 0; x < w; x++)
                                {
                                    gfx[x + y * w] = gfx[x + (y - n) * w];
                                }
                            }
                            for(int i = 0; i < n * w; i++)
                                gfx[i] = 0;

                            drawFlag = true;
                            pc += 2;
                            break;
                        }

                        case 0x00D0 ... 0x00DF:        // 0x00DN : Scroll up by N pixels (XO-CHIP)
                        {
                            int n = opcode & 0x000F;
                            int w = isHighRes ? 128 : 64;
                            int h = isHighRes ? 64 : 32;
                            for(int y = 0; y < h - n; ++y)
                            {
                                for(int x = 0; x < w; x++)
                                {
                                    gfx[x + y * w] = gfx[x + (y + n) * w];
                                }
                            }
                            for(int y = h - n; y < h; ++y)
                            {
                                for(int x = 0; x < w; x++)
                                {
                                    gfx[x + y * w] = 0;
                                }
                            }
                            drawFlag = true;
                            pc += 2;
                            break;
                        }

                        case 0x00FB:        // 0x00FB : Scroll right by 4 pixels
                        {
                            int w = isHighRes ? 128 : 64;
                            int h = isHighRes ? 64 : 32;
                            for(int y = 0; y < h; y++)
                            {
                                for(int x = w - 1; x >= 4; --x)
                                    gfx[x + y * w] = gfx[(x - 4) + y * w];
                                for(int x = 0; x < 4; x++)
                                    gfx[x + y * w] = 0;
                            }
                            drawFlag = true;
                            pc += 2;
                            break;
                        }

                        case 0x00FC:        // 0x00FC : Scroll left by 4 pixels
                        {
                            int w = isHighRes ? 128 : 64;
                            int h = isHighRes ? 64 : 32;
                            for(int y = 0; y < h; y++)
                            {
                                for(int x = 0; x < w - 4; ++x)
                                    gfx[x + y * w] = gfx[(x + 4) + y * w];
                                for(int x = w - 4; x < w; x++)
                                    gfx[x + y * w] = 0;
                            }
                            drawFlag = true;
                            pc += 2;
                            break;
                        }

                        case 0x00FE:        // 0x00FE : Disable extended screen mode (64x32)
                            isHighRes = false;
                            memset(gfx, 0, sizeof(gfx));
                            drawFlag = true;
                            pc += 2;
                            break;

                        case 0x00FF:        // 0x00FF : Enable extended screen mode
                            isHighRes = true;
                            memset(gfx, 0, sizeof(gfx));
                            drawFlag = true;
                            pc += 2;
                            break;
                        
                        default: 
                            std::cout << "Unknown opcode [0x0000] : 0x" << std::hex << opcode << std::dec << std::endl;
                            pc += 2;
                            break;
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
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            uint8_t vx = V[x];
            uint8_t vy = V[y];

            switch(opcode & 0x000F)
            {
                case 0x0000:V[x] = vy; // 0x8XY0 : puts value of Vy in Vx
                            break;

                case 0x0001:V[x] = vx | vy; // 0x8XY1 : performs bitwise OR Vx = Vx | Vy
                            break;

                case 0x0002:V[x] = vx & vy; // 0x8XY2 : performs bitwise AND Vx = Vx & Vy
                            break;

                case 0x0003:V[x] = vx ^ vy; // 0x8XY3 : performs XOR Vx = Vx ^ Vy
                            break;

                case 0x0004: // 0x8XY4 : Adds VY to VX
                {
                            uint16_t sum = vx + vy;
                            V[x] = sum & 0xFF;
                            V[0xF] = (sum > 0xFF) ? 1 : 0;
                            break;
                }

                case 0x0005: // 0x8XY5 : if Vx >= Vy then VF = 1 else Vf = 0, Vx = Vx - Vy
                            V[x] = vx - vy;
                            V[0xF] = (vx >= vy) ? 1 : 0;
                            break;

                case 0x0006: // 0x8XY6 : VIP behavior (shifts Vy)
                {
                            uint8_t carry = vy & 0x1;
                            V[x]  = vy >> 1;
                            V[0xF] = carry;
                            break;
                }

                case 0x0007: // 0x8XY7 : if Vy >= Vx then VF = 1 else Vf = 0 , Vx = Vy - Vx
                            V[x] = vy - vx;
                            V[0xF] = (vy >= vx) ? 1 : 0;
                            break;

                case 0x000E: // 0x8XYE : VIP behavior (shifts Vy)
                {
                            uint8_t carry = (vy & 0x80) >> 7;
                            V[x] = vy << 1;
                            V[0xF] = carry;
                            break;
                }

                default: std::cout << "Unknown opcode  [0x8000] :" << opcode ;
            }
            pc += 2;
            break;
        }
            
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
            unsigned short width = isHighRes ? 128 : 64;
            unsigned short height_screen = isHighRes ? 64 : 32;
            
            // Cast to int8_t to support negative coordinates for smooth clipping on left/top edges
            int8_t xCoord = V[(opcode & 0x0F00) >> 8];
            int8_t yCoord = V[(opcode & 0x00F0) >> 4];

            unsigned short height = opcode & 0x000F;
            bool is16x16 = (height == 0);
            if(is16x16) height = 16;

            V[0xF] = 0;     // Reset collision flag
            
            for(int y = 0; y < height; y++)
            {
                int curY = yCoord + y;

                // Strict Clipping
                if (curY < 0 || curY >= height_screen) continue;

                // 16x16 sprite  has 2 bytes per row : standard sprite has 1 bytes
                for(int b = 0; b < (is16x16 ? 2 : 1); b++)
                {
                    unsigned short pixel = memory[I + (is16x16 ? (y*2 + b) : y)];

                    for(int x = 0; x < 8; x++)
                    {
                        int curX = xCoord + (b*8) + x;

                        // Strict Clipping
                        if (curX < 0 || curX >= width) continue;

                        if((pixel & (0x80 >> x)) != 0)
                        {
                            int pos  = curX + (curY * width);

                            // Collision : pixel was already one
                            if(gfx[pos] == 1)
                                V[0xF] = 1;

                            // XOR to pixel
                            gfx[pos] ^= 1;
                        }
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
                                    I += ((opcode & 0x0F00) >> 8) + 1; // Original CHIP-8 increments I
                                    pc += 2;
                                    break;

                        case 0x0065:for(int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)     // 0xFX65 : Read register V0 - VX from memory starting at location I
                                    {
                                        V[i] = memory[I+i];
                                    }
                                    I += ((opcode & 0x0F00) >> 8) + 1; // Original CHIP-8 increments I
                                    pc += 2;
                                    break;
                    }
                    break;

            
        
        default:
            std::cout << "Uknown opcode" << opcode;
    }

}

void Chip8::updateTimers()
{
    if(delayTimer > 0)
        --delayTimer;

    if(soundTimer > 0)
        --soundTimer;
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
    if(size > (4096 - 512))
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
