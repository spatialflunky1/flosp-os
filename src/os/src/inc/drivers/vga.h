#ifndef VGA_H
#define VGA_H

#include <boot_font.h>
#include <drivers/mem.h>
#include <kernel/datatypes.h>

#define rgb(r,g,b) (r << 16) | (g << 8) | b

typedef struct {
    void*  FramebufferPointer;
    ui32_t HorizontalResolution;
    ui32_t VerticalResolution;
    ui32_t PixelsPerScanline;
} BOOT_VIDEO_MODE_INFO;

static BOOT_VIDEO_MODE_INFO* VideoModeInfo = NULL;

void initialize_video(BOOT_VIDEO_MODE_INFO* VMI);
void blank_output(void);
void kputchar(const unsigned char c);
void kscroll_down(void);
void kprint(const char* s);
void kprint_hex(ui64_t h, const ui8_t zero_x);

#endif
