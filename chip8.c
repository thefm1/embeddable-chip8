#include "chip8.h"


static u8 chip8_font[80] =
{
        0xF0,0x90,0x90,0x90,0xF0, // 0
        0x20,0x60,0x20,0x20,0x70, // 1
        0xF0,0x10,0xF0,0x80,0xF0, // 2
        0xF0,0x10,0xF0,0x10,0xF0, // 3
        0x90,0x90,0xF0,0x10,0x10, // 4
        0xF0,0x80,0xF0,0x10,0xF0, // 5
        0xF0,0x80,0xF0,0x90,0xF0, // 6
        0xF0,0x10,0x20,0x40,0x40, // 7
        0xF0,0x90,0xF0,0x90,0xF0, // 8
        0xF0,0x90,0xF0,0x10,0xF0, // 9
        0xF0,0x90,0xF0,0x90,0x90, // a
        0xE0,0x90,0xE0,0x90,0xE0, // b
        0xF0,0x80,0x80,0x80,0xF0, // c
        0xE0,0x90,0x90,0x90,0xE0, // d
        0xF0,0x80,0xF0,0x80,0xF0, // e
        0xF0,0x80,0xF0,0x80,0x80, // f
};

void chip8_setup(Chip8* chip, int (*rand)(void), void (*log)(const char* buffer), void (*beep)(void))
{
        
        chip->chip8_rand = rand;
        chip->chip8_log = log;
        chip->chip8_beep = beep;
        
        
}
Chip8 chip8_create(u8* ROM)
{
        Chip8 chip8 = {0};
        
        
        /* Setting up ROM. */
        for (u32 i =  0x200; i < 0xfff; i++) chip8.memory_map[i] = ROM[i - 0x200];
        /* Setting up the CHIP8 font */ 
        for (u32 i = 0x050; i < 0x9F; i++) chip8.memory_map[i] = chip8_font[i - 0x050];
        
        chip8.pc = 0x200;
        
        
        return chip8;
        
        
}



static u16 chip8_get_op(Chip8* chip8)
{
        return (chip8->memory_map[chip8->pc] << 8) | (chip8->memory_map[chip8->pc + 1]);
}



void chip8_emulate(Chip8* chip8)
{
        u16 op = chip8_get_op(chip8);
        
        switch (op & 0xf000)
        {
                
                case 0:
                {
                        switch ( op & 0x00ff)
                        {
                                case (0x00E0): 
                                {
                                        for (u32 h = 0; h < 32; h++)
                                        {
                                                for (u32 w = 0; w< 64;w++)
                                                {
                                                        chip8->framebuffer[w*h] = 0;
                                                }
                                                
                                        }
                                        chip8->pc += 2;
                                        chip8->chip8_log(" 00E0 - CLS\n" );
                                        
                                } break;
                                case (0x00EE): 
                                {
                                        
                                        chip8->sp -= 1;
                                        chip8->pc = chip8->stack[chip8->sp];
                                        chip8->pc += 2;
                                        chip8->chip8_log(" 00EE - RET\n" );
                                } break;
                                default:
                                {
                                        if (op & 0xf000 != 0){
                                                chip8->chip8_log("invalid operand.\n" );
                                        }
                                }
                        }
                        
                } break;
                
                case 0x1000:
                {
                        
                        s16 nnn = op & 0x0fff;
                        chip8->pc = nnn;
                        
                        chip8->chip8_log(" 1nnn - JP addr\n" );
                } break;
                
                case 0x2000:
                {
                        chip8->stack[(chip8->sp++)] = chip8->pc;
                        
                        s16 nnn = op & 0x0fff;
                        chip8->pc = nnn;
                        chip8->chip8_log(" 2nnn - CALL addr\n");
                } break;
                
                case 0x3000:
                {
                        s16 kk = op & 0x00ff;
                        s16 x  = (op & 0x0f00 )>> 8;
                        if (chip8->v[x] == kk)
                                chip8->pc += 4;
                        else 
                                chip8->pc += 2;
                        chip8->chip8_log(" 3xkk - SE Vx, byte\n");
                } break;
                
                case 0x4000:
                {
                        s16 kk = op & 0x00ff;
                        s16 x  = (op & 0x0f00 )>> 8;
                        if (chip8->v[x] != kk)
                                chip8->pc += 4;
                        else 
                                chip8->pc += 2;
                        chip8->chip8_log(" 4xkk - SNE Vx, byte\n");
                } break;
                
                case 0x5000:
                {
                        s16 x  = (op & 0x0f00 )>> 8;
                        s16 y  = (op & 0x00f0 )>> 4;
                        if (chip8->v[x] == chip8->v[y])
                                chip8->pc += 4;
                        else 
                                chip8->pc += 2;
                        chip8->chip8_log(" 5xy0 - SE Vx, Vy\n");
                } break;
                
                
                case 0x6000:
                {
                        s16 x = (op & 0x0f00) >> 8;
                        s16 nn = op & 0x00ff;
                        chip8->v[x] = nn;
                        
                        chip8->chip8_log(" 6xkk - LD Vx, byte\n");
                        chip8->pc += 2;
                } break;
                
                case 0x7000:
                {
                        s16 x  = (op & 0x0f00) >> 8;
                        s16 kk = (op & 0x00ff);
                        chip8->v[x] += kk;
                        chip8->pc += 2;
                        
                        chip8->chip8_log(" 7xkk - ADD Vx, byte\n");
                } break;
                
                case 0x8000:
                {
                        switch (op & 0x000f)
                        {
                                case 0x0000:{
                                        s16 x  = (op & 0x0f00) >> 8;
                                        s16 y  = (op & 0x00f0) >> 4;
                                        chip8->v[x] = chip8->v[y];
                                        chip8->pc += 2;
                                        
                                        chip8->chip8_log(" 8xy0 - LD Vx, Vy\n");
                                        
                                } break;
                                case 0x0001:{
                                        s16 x  = (op & 0x0f00) >> 8;
                                        s16 y  = (op & 0x00f0) >> 4;
                                        chip8->v[x] = chip8->v[x]  |  chip8->v[y];
                                        chip8->pc += 2;
                                        
                                        chip8->chip8_log(" Set Vx = Vx OR Vy.\n");
                                } break;
                                case 0x0002:{
                                        s16 x  = (op & 0x0f00) >> 8;
                                        s16 y  = (op & 0x00f0) >> 4;
                                        chip8->v[x] &= chip8->v[y];
                                        chip8->pc += 2;
                                        
                                        chip8->chip8_log(" Set Vx = Vx AND Vy.\n");
                                } break;
                                case 0x0003:{
                                        s16 x  = (op & 0x0f00) >> 8;
                                        s16 y  = (op & 0x00f0) >> 4;
                                        chip8->v[x] ^= chip8->v[y];
                                        chip8->pc += 2;
                                        
                                        chip8->chip8_log(" 8xy3 - XOR Vx, Vy\n");
                                } break;
                                case 0x0004:{
                                        s16 x  = (op & 0x0f00) >> 8;
                                        s16 y  = (op & 0x00f0) >> 4;
                                        chip8->v[x] += chip8->v[y];
                                        chip8->v[0xF] = chip8->v[y] >  (0xff - chip8->v[x]) ? 1 : 0;
                                        chip8->pc += 2;
                                        
                                        chip8->chip8_log(" 8xy4 - ADD Vx, Vy\n");
                                } break;
                                case 0x0005:{
                                        s16 x  = (op & 0x0f00) >> 8;
                                        s16 y  = (op & 0x00f0) >> 4;
                                        chip8->v[0xF] = chip8->v[y] > chip8->v[x] ? 0 : 1;
                                        chip8->v[x] -= chip8->v[y];
                                        
                                        chip8->pc += 2;
                                        chip8->chip8_log(" 8xy5 - SUB Vx, Vy\n");
                                } break;
                                case 0x0006:{
                                        s16 x  = (op & 0x0f00) >> 8;
                                        
                                        chip8->v[0xF] = chip8->v[x] & 0x1;
                                        chip8->v[x] >>= 1;
                                        chip8->pc += 2;
                                        chip8->chip8_log(" 8xy6 - SHR Vx {, Vy}\n");
                                } break;
                                case 0x0007:{
                                        s16 x  = (op & 0x0f00) >> 8;
                                        s16 y  = (op & 0x00f0) >> 4;
                                        
                                        chip8->v[0xF] = chip8->v[x] > chip8->v[y] ? 0 : 1;
                                        chip8->v[x] =  chip8->v[y] -  chip8->v[x];
                                        chip8->pc += 2;
                                        chip8->chip8_log(" 8xy7 - SUBN Vx, Vy\n");
                                } break;
                                case 0x000e:{
                                        s16 x  = (op & 0x0f00) >> 8;
                                        
                                        chip8->v[0xF] = chip8->v[x] >> 7;
                                        chip8->v[x] <<= 1;
                                        chip8->pc += 2;
                                        chip8->chip8_log(" 8xy6 - SHR Vx {, Vy}\n");
                                } break;
                                default :
                                {
                                        chip8->chip8_log("invalid operand.\n" );
                                        chip8->pc += 2;
                                } break;
                        }
                } break;
                case 0x9000:
                {
                        s16 x  = (op & 0x0f00 )>> 8;
                        s16 y  = (op & 0x00f0 )>> 4;
                        if (chip8->v[x] != chip8->v[y])
                                chip8->pc += 4;
                        else
                                chip8->pc += 2;
                        
                        chip8->chip8_log(" Skip next instruction if Vx != Vy.\n");
                        
                        
                } break;
                case 0xA000:
                {
                        s16 nnn = op & 0x0fff;
                        chip8->pc += 2;
                        chip8->I = nnn;
                        chip8->chip8_log(" Annn - LD I, addr\n");
                } break;
                case 0xB000:
                {
                        s16 nnn = op & 0x0fff;
                        chip8->pc = nnn + chip8->v[0];
                        
                        
                        chip8->chip8_log(" Bnnn - JP V0, addr\n");
                } break;
                
                case 0xC000:
                {
                        s16 kk = op & 0x00ff;
                        chip8->v[(op & 0x0f00) >> 8] =( chip8->chip8_rand() %  255 + 1) & kk;
                        chip8->pc += 2;
                        chip8->chip8_log(" Cxkk - RND Vx, byte\n");
                } break;
                
                
                case 0XD000:
                {
                        u8 vx  = chip8->v[(op & 0x0f00) >> 8];
                        u8 vy  = chip8->v[(op & 0x00f0) >> 4];
                        u8 n = op & 0x000f;
                        u8 pixel;
                        chip8->v[0xF] = 0;
                        
                        for (u32 y = 0; y < n; y++)
                        {
                                pixel = chip8->memory_map[chip8->I + y];
                                
                                for (u32 x = 0; x < 8; x++){
                                        if (pixel & (0x80 >> x)){
                                                if (chip8->framebuffer[x+vx+(y + vy) * 64] )
                                                        chip8->v[0xF] = 1;
                                                
                                                chip8->framebuffer[x+vx+(y + vy) * 64]^= 1;
                                                
                                        }
                                }
                        }
                        chip8->pc += 2;
                        chip8->flags.draw = 1;
                        chip8->chip8_log(" Dxyn - DRW Vx, Vy, nibble\n");
                } break;
                
                case 0XE000:
                {
                        
                        
                        switch (op & 0x00ff)
                        {
                                case 0x009E:
                                {
                                        
                                        
                                        if (chip8->keys[chip8->v[(op & 0x0f00) >> 8]] == 1)
                                                
                                                chip8->pc += 2;
                                        
                                        chip8->chip8_log(" Ex9E - SKP Vx\n");
                                } break;
                                case 0x00A1:
                                {
                                        
                                        if (chip8->keys[chip8->v[(op & 0x0f00) >> 8]] == 0 )
                                                
                                                
                                                chip8->pc += 2;
                                        
                                        
                                        chip8->chip8_log(" ExA1 - SKNP Vx\n");
                                } break;
                                
                                default: {
                                        
                                        
                                        chip8->chip8_log(" Err: Invalid Opcode\n");
                                } break;
                        }
                        
                        chip8->pc += 2;
                } break;
                
                
                case 0XF000:
                {
                        
                        switch (op & 0x00ff)
                        {
                                
                                case  0x0007:
                                {
                                        u16 x = ( op & 0x0f00 )>> 8;
                                        chip8->v[x] = chip8->delay_timer;
                                        chip8->pc += 2;
                                        chip8->chip8_log(" Fx07 - LD Vx, DT\n");
                                } break;
                                case  0x000A:
                                {
                                        
                                        
                                        for (int i=0;i<16;i++){
                                                if (chip8->keys[i] != 0){
                                                        chip8->v[(op & 0x0f00) >> 8] = i;
                                                        
                                                        chip8->pc += 2;
                                                }
                                        }
                                        
                                        chip8->chip8_log(" Fx0A - LD Vx, K\n");
                                        
                                } break;
                                case  0x0015:
                                {
                                        u16 x = ( op & 0x0f00 )>> 8;
                                        chip8->delay_timer = chip8->v[x];
                                        chip8->pc += 2;
                                        chip8->chip8_log(" Fx15 - LD DT, Vx\n");
                                } break;
                                case  0x0018:
                                {
                                        u16 x = ( op & 0x0f00 )>> 8;
                                        chip8->sound_timer = chip8->v[x];
                                        chip8->pc += 2;
                                        chip8->chip8_log(" Fx18 - LD ST, Vx\n");
                                } break;
                                
                                case  0x001E:
                                {
                                        u16 x = ( op & 0x0f00 )>> 8;
                                        chip8->v[0xf] = chip8->I + chip8->v[x] > 0xfff ? 1 : 0;
                                        chip8->I += chip8->v[x];
                                        chip8->pc += 2;
                                        chip8->chip8_log(" Fx1E - ADD I, Vx\n");
                                } break;
                                case  0x0029:
                                {
                                        u16 x = ( op & 0x0f00 )>> 8;
                                        chip8->I = chip8->v[x] * 0x5;
                                        chip8->pc += 2;
                                        chip8->chip8_log(" Fx29 - LD F, Vx\n");
                                } break;
                                
                                case  0x0033:
                                {
                                        u16 x = ( op & 0x0f00 )>> 8;
                                        chip8->memory_map[chip8->I] = chip8->v[x] / 100;
                                        chip8->memory_map[chip8->I + 1] = (chip8->v[x] / 10) % 10;
                                        chip8->memory_map[chip8->I + 2] = (chip8->v[x]) % 10;
                                        chip8->pc += 2;
                                        chip8->chip8_log(" Fx33 - LD B, Vx\n");
                                } break;
                                
                                case  0x0055:
                                {
                                        u16 x = ( op & 0x0f00 )>> 8;
                                        for (s32 i = 0; i <= x; ++i)
                                                chip8->memory_map[chip8->I + i] = chip8->v[i];
                                        chip8->I +=  chip8->v[x] + 1;
                                        chip8->pc += 2;
                                        chip8->chip8_log(" Fx55 - LD [I], Vx\n");
                                } break;
                                
                                case  0x0065:
                                {
                                        u16 x = ( op & 0x0f00 )>> 8;
                                        for (s32 i = 0; i <= x; ++i)
                                                chip8->v[i] = chip8->memory_map[chip8->I + i];
                                        chip8->I +=  chip8->v[x] + 1;
                                        chip8->pc += 2;
                                        chip8->chip8_log(" Fx65 - LD Vx, [I]\n");
                                } break;
                                
                                default:
                                {
                                        chip8->chip8_log("Err: Invalid opcode\n");
                                        chip8->pc += 2;
                                } break;
                        }
                        
                }break;
                
                default:
                {
                        
                        chip8->chip8_log("Err: Invalid opcode\n");
                        chip8->pc += 2;
                }
        }
        
        
        if (chip8->pc > 0xfff)
                
        {
                chip8->chip8_beep();
                chip8->chip8_log("Exceeded Memory violation!\n");
                chip8->chip8_log("PC: The program counter has reached a value greater than 0xfff!\n");
                chip8->chip8_log("==========================\n");
                for (;;);
        }
        if (chip8->sound_timer > 0)
        {
                chip8->chip8_beep();
                chip8->sound_timer --;
        }
        if (chip8->delay_timer > 0)
                chip8->delay_timer --;
        
        
        
}