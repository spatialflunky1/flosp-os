#ifndef VGA_H
#define VGA_H

#include <boot_font.h>
#include <drivers/mem.h>
#include <datatypes.h>

#define rgb(r,g,b) (r << 16) | (g << 8) | b

typedef struct {
    void*  FramebufferPointer;
    void*  BackbufferPointer;
    ui32_t HorizontalResolution;
    ui32_t VerticalResolution;
    ui32_t PixelsPerScanline;
} BOOT_VIDEO_MODE_INFO;

typedef struct {
    ui32_t width;
    ui32_t height;
    ui32_t pitch;
    ui32_t depth;
} FRAMEBUFFER_INFO_T;

void initialize_video(BOOT_VIDEO_MODE_INFO* VMI);
void flush_buffer(void);
void blank_output(void);
void kputchar(const unsigned char c);
void kscroll_down(void);
void kprint(const char* s);
void kprint_hex(ui64_t h, const bool zero_x);
void kprint_num(ui64_t n);
void kprint_bin(ui64_t b, ui8_t size_b);
void backspace(void);
void* get_framebuffer_address(void);
void* get_backbuffer_address(void);

#endif
