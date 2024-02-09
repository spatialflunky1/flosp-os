#ifndef DRIVERS_H
#include "drivers.h"
#endif

#define VIDEO_MEM 0xB8000
#define WF_BB 0x0F
#define VGA_CTRL 0x3D4
#define VGA_DATA 0x3D5

void reset_output();
void kprint_8024(const char* str);
void disable_cursor();
void enable_cursor();
