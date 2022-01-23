
#ifndef CHIP8_H
#define CHIP8_H



typedef signed int      s32;
typedef unsigned int    u32;
typedef signed char     s8;
typedef unsigned char   u8;
typedef unsigned short  s16;
typedef unsigned short  u16;

typedef struct Chip8 Chip8;

struct Chip8
{
        
        u8 memory_map[4096];
        /* 0x000 - 0x200 : reserved */
        
        u8  v[16];
        u16 I;
        u8 sound_timer;
        u8 delay_timer;
        
        
        u16 pc;
        u16 sp;
        u16 stack[16];
        
        u8 framebuffer[32 * 64];
        u8 keys[16];
        
        struct 
        {
                u8 draw;
                
        } flags;
        int (*chip8_rand)(void);
        void (*chip8_log)(const char* buffer);
        void (*chip8_beep)(void);
        
        
        
        
};

Chip8 chip8_create(u8* ROM);
void chip8_setup(Chip8* chip, int (*rand)(void), void (*log)(const char* buffer), void (*beep)(void));
void chip8_emulate(Chip8* chip8);



#endif