#include <drivers/vga.h>

//
// Global Vars
//
// Output info
ui16_t output_height = BOOT_HEIGHT;
ui16_t output_width = BOOT_WIDTH;
// Text info
ui16_t text_col_px = 0;
ui8_t text_line = 0;
int text_color = rgb(255,255,255);
// Font info
ui8_t font_size = 16; // height

void initialize_video(BOOT_VIDEO_MODE_INFO* VMI) {
    VideoModeInfo = VMI;
    blank_output();
}

void blank_output(void) {
    ui32_t* pos;
    for (ui64_t row = 0; row < VideoModeInfo->VerticalResolution; row++) {
        for (ui64_t col = 0; col < VideoModeInfo->HorizontalResolution; col++) {
            pos = VideoModeInfo->FramebufferPointer;
            // Set row
            pos += row * VideoModeInfo->HorizontalResolution;
            // Set cold
            pos += col;

            // Set color
            *pos = rgb(0,0,0);
        }
    }
}

void kputchar(const unsigned char c) {     
    ui32_t* pos = VideoModeInfo->FramebufferPointer;
    // Advance to current line
    pos += text_line * font_size * VideoModeInfo->HorizontalResolution;
    // Advance to current column
    pos += ((text_col_px)  + 8) - 1; // font width is always 8;
                                      // algorithm starts at the last pixel of the line
    ui32_t* last_pos = pos; // pos at the beginning of the line
    const unsigned char* ch_addr;
    switch (font_size) {
        case 8:
            ch_addr = &vgafont8[c * font_size];
            break;
        case 14:
            ch_addr = &vgafont14[c * font_size];
            break;
        case 16:
            ch_addr = &vgafont16[c * font_size];
            break;
        default:
            return;
    }
    for (unsigned int i = 0; i < font_size; i++) { 
        unsigned char ch = *ch_addr;
        while (ch != 0) {
            if (ch % 2 == 1) { // last bit is set
                *pos = text_color;
            }
            ch = ch >> 1;
            pos--;
        }
        ch_addr++;
        last_pos += VideoModeInfo->HorizontalResolution;
        pos = last_pos;
    }
    text_col_px += 9; // 9 instead of 8 to add a space in between chars printed
}

void kprint(const char *s) {
    while (*s != '\0') {
        if (*s == '\n') {
            text_col_px = 0;
            text_line += 1;
        }
        else {
            kputchar(*s);
        }
        s++;
    }
}
