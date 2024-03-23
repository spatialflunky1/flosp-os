#include <drivers/vga.h>

// Global Vars
ui8_t line_loc = 0;
ui16_t curs_loc = 0;
ui16_t curs_enable = 1;

void clear_output_8025(void) {
    // Fill screen with blank chars
    ui8_t* video_loc = (ui8_t*) VIDEO_8025_MEM;
    // 80x25 columns
    for (ui16_t i=0; i<80*25; i++) {
        video_loc[0] = ' ';
        video_loc[1] = WF_BB;
        video_loc+=2;
    }
    // Reset cursor pos
    if (curs_enable) {
        set_cursor_pos(0);
    }
}

void kputchar_8025(const char c, ui8_t color, ui8_t* video_loc) {
    video_loc[0] = c;
    video_loc[1] = color;
}

void kprint_8025(const char* str) {
    ui8_t* video_loc = (ui8_t*) VIDEO_8025_MEM + (curs_loc*2);
    while ((*str)!=0) {
        if (*str == '\n') {
            line_loc++;
            curs_loc = line_loc * COLS;
            video_loc = (ui8_t*) VIDEO_8025_MEM + (curs_loc*2);
        }
        else {
            kputchar_8025(*str, WF_BB, video_loc);
            video_loc+=2;
            curs_loc++;
        }
        if (curs_loc >= (ROWS*COLS)) {
            line_loc--;
            scroll_down_8025();
            video_loc = (ui8_t*) VIDEO_8025_MEM + (curs_loc*2);
        }
        str++;
    }
    if (curs_enable) {
        set_cursor_pos(curs_loc);
    }
}

void kprint_num_8025(ui64_t num) {
    ui8_t* video_loc = (ui8_t*) VIDEO_8025_MEM + (curs_loc*2);
    char   char_stack[UI64_T_MAX_DEC_DIGITS];
    i8_t   stack_top = -1;
    if (num == 0) {
        stack_top++;
        char_stack[stack_top] = '0';
    }
    while (num != 0) {
        stack_top++;
        char_stack[stack_top] = (char)((num%10)+48);
        num /= 10;
    }
    while (stack_top >= 0) {
        kputchar_8025(char_stack[stack_top], WF_BB, video_loc);
        video_loc+=2;
        curs_loc++;
        stack_top--;
    }
    if (curs_enable) {
        set_cursor_pos(curs_loc);
    }
}

void kprint_hex_8025(ui64_t num) {
    kprint_8025("0x");
    ui8_t* video_loc = (ui8_t*) VIDEO_8025_MEM + (curs_loc*2);
    char   tmp;
    char   char_stack[UI64_T_MAX_HEX_DIGITS];
    i8_t   stack_top = -1;
    if (num == 0) {
        stack_top++;
        char_stack[stack_top] = '0';
    }
    while (num != 0) {
        tmp = (char)(num & 0xF);
        tmp = (tmp >= 10) ? tmp+55 : tmp+48;
        stack_top++;
        char_stack[stack_top] = tmp;
        num /= 16;
    }
    while (stack_top >= 0) {
        kputchar_8025(char_stack[stack_top], WF_BB, video_loc);
        video_loc+=2;
        curs_loc++;
        stack_top--;
    }
    if (curs_enable) {
        set_cursor_pos(curs_loc);
    }
}

void disable_cursor(void) {
    outb(VGA_CTRL, 0x0A);
    outb(VGA_DATA, 0x20); // set bit 5 00100000
    curs_enable = 0;
}

void enable_cursor(void) {
    // Enable Cursor
    outb(VGA_CTRL, 0x0A);
    outb(VGA_DATA, 0x0E); // unset bit 5, bits 0-4 specifies the scan line for the cursor to begin
    // Update Position
    set_cursor_pos(curs_loc);
    curs_enable = 1;
}

void set_cursor_pos(ui16_t pos) {
    outb(VGA_CTRL, 0x0F);                           // Sets 0x3D5 to hold the low byte
    outb(VGA_DATA, (ui8_t)(pos & 0x00FF));  // Set the low byte of position
    outb(VGA_CTRL, 0x0E);                           // Sets 0x3D5 to hold the high byte1 
    outb(VGA_DATA, (ui8_t)(pos >> 8));      // Sets the high byte of position
}

void scroll_down_8025(void) {
    ui8_t* video_loc = (ui8_t*) VIDEO_8025_MEM + (COLS*2); // start at 2nd line
    for (ui16_t i = 0; i < ROWS-1; i++) {
        memcpy(video_loc, video_loc-(COLS*2), COLS*2);
        video_loc+=(COLS*2); // advance 1 row
    }
    // blank last line
    video_loc = (ui8_t*) VIDEO_8025_MEM + (COLS*(ROWS-1)*2);
    for (ui8_t i = 0; i < COLS; i++) {
        video_loc[0] = 0;
        video_loc+=2;
    }
    curs_loc-=80;
    if (curs_enable) {
        set_cursor_pos(curs_loc);
    }
}

#define TEST_COLOR 0x00FF00FF
void blank_output(BOOT_VIDEO_MODE_INFO* VideoModeInfo) {
    ui32_t* pos;
    for (ui64_t row = 0; row < VideoModeInfo->VerticalResolution; row++) {
        for (ui64_t col = 0; col < VideoModeInfo->HorizontalResolution; col++) {
            pos = VideoModeInfo->FramebufferPointer;
            // Set row
            pos += row * VideoModeInfo->PixelsPerScanline;
            // Set cold
            pos += col;

            // Set color
            *pos = rgb(0,0,0);
        }
    }
}
