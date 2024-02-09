#include "vga.h"

// Global Vars
unsigned char line_loc = 0;
unsigned short curs_loc = 0;
unsigned short curs_enable = 1;

void reset_output() {
    // Fill screen with blank chars
    unsigned char* VIDEO_LOC = (unsigned char*) VIDEO_MEM;
    // 80x25 columns
    for (int i=0; i<80*25; i++) {
        VIDEO_LOC[0] = ' ';
        VIDEO_LOC[1] = WF_BB;
        VIDEO_LOC+=2;
    }
    // Reset cursor pos
    outb(VGA_CTRL, 0x0F); // Sets 0x3D5 to hold the low byte
    outb(VGA_DATA,    0); // Set the low byte of position
    outb(VGA_CTRL, 0x0E); // Sets 0x3D5 to hold the high byte
    outb(VGA_DATA,    0); // Sets the high byte of position
}

void kprint_8024(const char* str) {
    unsigned char* video_loc = (unsigned char*) VIDEO_MEM + (curs_loc*2);
    while ((*str)!=0) {
        if (*str == '\n') {
            line_loc++;
            curs_loc = line_loc * 80;
            video_loc = (unsigned char*) VIDEO_MEM + (curs_loc*2);
        }
        else {
            video_loc[0] = *str;
            video_loc[1] = WF_BB;
            video_loc+=2;
            curs_loc++;
        }
        str++;
    }
    if (curs_enable) {
        outb(VGA_CTRL, 0x0F);
        outb(VGA_DATA, (unsigned char)(curs_loc & 0x00FF));
        outb(VGA_CTRL, 0x0E);
        outb(VGA_DATA, (unsigned char)(curs_loc >> 8));
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
    outb(VGA_CTRL, 0x0F);
    outb(VGA_DATA, (unsigned char)(curs_loc & 0x00FF));
    outb(VGA_CTRL, 0x0E);
    outb(VGA_DATA, (unsigned char)(curs_loc >> 8));
    curs_enable = 1;
}
