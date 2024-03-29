#ifndef VGA_H
#define VGA_H

#include <drivers/boot_font.h>
#include <kernel/datatypes.h>

#define BOOT_HEIGHT 600
#define BOOT_WIDTH  800

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
void kprint(const char* s);

#endif
