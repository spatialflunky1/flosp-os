#ifndef DRIVERS_H
#include "drivers.h"
#endif
#ifndef MEM_H
#include "mem.h"
#endif

#define VIDEO_8025_MEM 0xB8000
#define VIDEO_8025_MEM_END VIDEO_8025_MEM+(80*25*2)-2
#define ROWS 25
#define COLS 80
#define WF_BB 0x0F
#define VGA_CTRL 0x3D4
#define VGA_DATA 0x3D5

void clear_output_8025();
void kprint_8025(const char* str);
void disable_cursor();
void enable_cursor();
void set_cursor_pos(unsigned short pos);
void scroll_down_8025();
