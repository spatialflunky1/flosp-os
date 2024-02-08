#ifndef DRIVERS_H
#include "drivers.h"
#endif

#define VIDEO_MEM 0xB8000
#define WF_BB 0x0F

void reset_output();
void print(const char* str);
void disable_cursor();
void enable_cursor();
