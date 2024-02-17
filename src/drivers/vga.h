#ifndef VGA_H
#define VGA_H

#include "io.h"
#include "mem.h"

#define VIDEO_8025_MEM 0xB8000
#define VIDEO_8025_MEM_END VIDEO_8025_MEM+(ROWS*COLS*2)-2
#define ROWS 25
#define COLS 80
#define WF_BB 0x0F
#define VGA_CTRL 0x3D4
#define VGA_DATA 0x3D5

void clear_output_8025(void);
void kputchar_8025(const char c, unsigned char color, unsigned char* video_loc);
void kprint_8025(const char* str);
void kprint_num_8025(unsigned int num);
void disable_cursor(void);
void enable_cursor(void);
void set_cursor_pos(unsigned short pos);
void scroll_down_8025(void);

#endif
