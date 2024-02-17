#ifndef VGA_H
#define VGA_H

#include <drivers/io.h>
#include <drivers/mem.h>
#include <kernel/datatypes.h>

#define VIDEO_8025_MEM 0xB8000
#define VIDEO_8025_MEM_END VIDEO_8025_MEM+(ROWS*COLS*2)-2
#define ROWS 25
#define COLS 80
#define WF_BB 0x0F
#define VGA_CTRL 0x3D4
#define VGA_DATA 0x3D5

void clear_output_8025(void);
void kputchar_8025(const char c, ui8_t color, ui8_t* video_loc);
void kprint_8025(const char* str);
void kprint_num_8025(ui64_t num);
void disable_cursor(void);
void enable_cursor(void);
void set_cursor_pos(ui16_t pos);
void scroll_down_8025(void);

#endif
