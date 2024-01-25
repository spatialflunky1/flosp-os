#define VIDEO_MEM 0xB8000
#define WF_BB 0x0F

void outb(unsigned char value, unsigned short port) {
    __asm__("out %0,%1"
            : // No outputs
            : "r" (value), "r" (port));
}

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
    outb(0x0F, 0x3D4); // Sets 0x3D5 to hold the low byte
    outb(0,    0x3D5); // Set the low byte of position
    outb(0x0E, 0x3D4); // Sets 0x3D5 to hold the high byte
    outb(0,    0x3D5); // Sets the high byte of position

}

void main(void) {
    // reset_output();
    for(;;);
}
