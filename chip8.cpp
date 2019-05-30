/**
 * 
*/
#include <stdio.h>

class chip8
{
    private:

    // first define memory elements
    unsigned short opcode; // 2 bytes
    unsigned char memory[4096]; // 4K bytes
    unsigned char V[16]; // 16 * 1byte registers
    unsigned short I; // Index register (2 bytes)
    unsigned short pc; // program counter (2 bytes)
    unsigned char gfx[64 * 32]; // pixel screen (2048 * 2bytes)
    unsigned char delay_timer, sound_timer;
    unsigned short stack[16];
    unsigned short sp; // stack pointer
    unsigned char key[16]; // HEX keypad input
    unsigned char chip8_fontset[80] =
    { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    
    public:

    chip8()
    {
        // initialize registers and memory once
        pc = 0x200; // Program counter start at 0x200
        opcode = 0; // Reset current opcode
        I = 0;      // Reset index register
        sp = 0;     // Reset stack pointer

        // Clear display
        // Clear stack
        // Clear registers V0-VF
        // Clear memory

        // Load fontset
        for (int  i = 0; i < 80; i++)
            memory[i] = chip8_fontset[i];
        
        // Reset timers
    }

    void loadProgram(char *buffer, int bufferSize)
    {
        for(int i = 0; i < bufferSize; i++)
            memory[i + 512] = buffer[i]; // 512 = 0x200
    }

    void emulateCycle()
    {
        // Fetch Opcode
        opcode = memory[pc] << 8 | memory[pc + 1];
        
        // Decode Opcode
        switch(opcode & 0xF000)
        {
            // Execute Opcode
            case 0x0000:
                switch (opcode & 0x000F)
                {
                case 0x00EE:
                break;
                
                case 0x000E: // 0x00E0: Clears the screen
                break;
                
                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
                break;
                }
            case 0x2000:
                stack[sp] = pc;
                sp++;
                pc = opcode & 0x0FFF;
            break;
            case 0x8000:
                switch(opcode & 0x000F)
                {
                    case 0x0004:
                        if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00)]))
                            V[0xF] = 1; // carry
                        else
                            V[0xF] = 0;
                        V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    break;
                    default:
                    break;
                }
            break;
            case 0xA000: // Annn: Set I to the address nnn
                I = opcode & 0x0FFF;
                pc += 2;
            break;
            case 0xD000:
            {
                unsigned short x = V[(opcode & 0x0F00) >> 8];
                unsigned short y = V[(opcode & 0x00F0) >> 4];
                unsigned short height = opcode & 0x000F;
                unsigned short pixel;
                V[0xF] = 0;
                for (int yline = 0; yline < height; yline++)
                {
                    pixel = memory[I + yline];
                    for (int xline = 0; xline < 8; xline++)
                    {
                        if ((pixel & (0x80 >> xline)) != 0)
                        {
                            if(gfx[(x + xline + ((y + yline) * 64))] == 1)
                                V[0xF] = 1;
                            gfx[x + xline + ((y + yline) * 64)] ^= 1;
                        }
                    }
                }

                // drawFlag = true;
                pc += 2;
            }
            break;
            case 0xE000:
                switch(opcode & 0x00FF)
                {
                    case 0x009E:
                        if (key[V[(opcode & 0x0F00) >> 8]] != 0)
                            pc += 4;
                        else
                            pc += 2;
                    break;
                    default:
                    break;
                }
            break;
            case 0xF000:
                switch(opcode & 0x00FF)
                {
                    case 0x0033: // LD B, Vx
                        memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                        memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                        memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    break;
                }
            break;
            default:
                printf("Unknown opcode: 0x%X\n", opcode);
        }

        // Update timers
        if (delay_timer > 0)
            delay_timer--;
        
        if (sound_timer > 0)
        {
            if (sound_timer == 1)
                printf("BEEP!\n");
            sound_timer--;
        }
    }
};