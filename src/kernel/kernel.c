#define VIDEO_MEM 0xB8000
#define WF_BB 0x0F

// Global Vars
unsigned char LINE_LOC = 0;
unsigned short CURS_LOC = 0;


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

void print(const char* str) {
    unsigned char* VIDEO_LOC = (unsigned char*) VIDEO_MEM + (CURS_LOC*2);
    while ((*str)!=0) {
        if (*str == '\n') {
            LINE_LOC++;
            CURS_LOC = LINE_LOC * 80;
            VIDEO_LOC = (unsigned char*) VIDEO_MEM + (CURS_LOC*2);
        }
        else {
            VIDEO_LOC[0] = *str;
            VIDEO_LOC[1] = WF_BB;
            VIDEO_LOC+=2;
            CURS_LOC++;
        }
        str++;
    }
    outb(0x0F, 0x3D4);
    outb(CURS_LOC & 0x00FF, 0x3D5);
    outb(0x0E, 0x3D4);
    outb(CURS_LOC & 0xFF00, 0x3D5);
}

void main(void) {
    reset_output();
    print("Kernel flosp-");
    print(KVER);
    print(" loaded\n\0");
    for(;;);
}
