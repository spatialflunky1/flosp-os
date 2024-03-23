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

#define rgb(r,g,b) (r << 16) | (g << 8) | b

typedef struct {
    void*  FramebufferPointer;
    ui32_t HorizontalResolution;
    ui32_t VerticalResolution;
    ui32_t PixelsPerScanline;
} BOOT_VIDEO_MODE_INFO;

extern ui16_t curs_loc;

void clear_output_8025(void);
void kputchar_8025(const char c, ui8_t color, ui8_t* video_loc);
void kprint_8025(const char* str);
void kprint_num_8025(ui64_t num);
void kprint_hex_8025(ui64_t num);
void disable_cursor(void);
void enable_cursor(void);
void set_cursor_pos(ui16_t pos);
void scroll_down_8025(void);
void blank_output(BOOT_VIDEO_MODE_INFO* VideoModeInfo);

#endif
