#include "vga.h"

// Global Vars
unsigned char line_loc = 0;
unsigned short curs_loc = 0;
unsigned short curs_enable = 1;

void clear_output_8025() {
    // Fill screen with blank chars
    unsigned char* video_loc = (unsigned char*) VIDEO_8025_MEM;
    // 80x25 columns
    for (int i=0; i<80*25; i++) {
        video_loc[0] = ' ';
        video_loc[1] = WF_BB;
        video_loc+=2;
    }
    // Reset cursor pos
    if (curs_enable) {
        set_cursor_pos(0);
    }
}

void kputchar_8025(const char c, unsigned char color, unsigned char* video_loc) {
    video_loc[0] = c;
    video_loc[1] = color;
}

void kprint_8025(const char* str) {
    unsigned char* video_loc = (unsigned char*) VIDEO_8025_MEM + (curs_loc*2);
    while ((*str)!=0) {
        if (*str == '\n') {
            line_loc++;
            curs_loc = line_loc * COLS;
            video_loc = (unsigned char*) VIDEO_8025_MEM + (curs_loc*2);
        }
        else {
            kputchar_8025(*str, WF_BB, video_loc);
            video_loc+=2;
            curs_loc++;
        }
        if (curs_loc >= (ROWS*COLS)) {
            line_loc--;
            scroll_down_8025();
            video_loc = (unsigned char*) VIDEO_8025_MEM + (curs_loc*2);
        }
        str++;
    }
    if (curs_enable) {
        set_cursor_pos(curs_loc);
    }
}

void kprint_num_8025(unsigned int num) {
    unsigned int tmp = num;
    unsigned char* video_loc = (unsigned char*) VIDEO_8025_MEM + (curs_loc*2);
    while (tmp != 0) {
        kputchar_8025((char)((tmp%10)+48), WF_BB, video_loc);
        tmp /= 10;
        video_loc+=2;
        curs_loc++;
    }
}

void disable_cursor() {
    outb(VGA_CTRL, 0x0A);
    outb(VGA_DATA, 0x20); // set bit 5 00100000
    curs_enable = 0;
}

void enable_cursor() {
    // Enable Cursor
    outb(VGA_CTRL, 0x0A);
    outb(VGA_DATA, 0x0E); // unset bit 5, bits 0-4 specifies the scan line for the cursor to begin
    // Update Position
    set_cursor_pos(curs_loc);
    curs_enable = 1;
}

void set_cursor_pos(unsigned short pos) {
    outb(VGA_CTRL, 0x0F);                           // Sets 0x3D5 to hold the low byte
    outb(VGA_DATA, (unsigned char)(pos & 0x00FF));  // Set the low byte of position
    outb(VGA_CTRL, 0x0E);                           // Sets 0x3D5 to hold the high byte1 
    outb(VGA_DATA, (unsigned char)(pos >> 8));      // Sets the high byte of position
}

void scroll_down_8025() {
    unsigned char* video_loc = (unsigned char*) VIDEO_8025_MEM + (COLS*2); // start at 2nd line
    for (int i = 0; i < ROWS-1; i++) {
        memcpy(video_loc, video_loc-(COLS*2), COLS*2);
        video_loc+=(COLS*2); // advance 1 row
    }
    // blank last line
    video_loc = (unsigned char*) VIDEO_8025_MEM + (COLS*(ROWS-1)*2);
    for (int i = 0; i < COLS; i++) {
        video_loc[0] = 0;
        video_loc+=2;
    }
    curs_loc-=80;
    if (curs_enable) {
        set_cursor_pos(curs_loc);
    }
}
