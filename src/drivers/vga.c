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
    outb(0x3D4, 0x0F); // Sets 0x3D5 to hold the low byte
    outb(0x3D5,    0); // Set the low byte of position
    outb(0x3D4, 0x0E); // Sets 0x3D5 to hold the high byte
    outb(0x3D5,    0); // Sets the high byte of position
}

void print(const char* str) {
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
        outb(0x3D4, 0x0F);
        outb(0x3D5, curs_loc & 0x00FF);
        outb(0x3D4, 0x0E);
        outb(0x3D5, curs_loc & 0xFF00);
    }
}

void disable_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20); // set bit 5 00100000
    curs_enable = 0;
}

void enable_cursor() {
    // Enable Cursor
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x0E); // unset bit 5, bits 0-4 specifies the scan line for the cursor to begin
    // Update Position
    outb(0x3D4, 0x0F);
    outb(0x3D5, curs_loc & 0x00FF);
    outb(0x3D4, 0x0E);
    outb(0x3D5, curs_loc & 0xFF00);
    curs_enable = 1;
}
