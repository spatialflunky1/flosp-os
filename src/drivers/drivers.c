void outb(unsigned char value, unsigned short port) {
    __asm__("out %%al,%%dx"
            : // No outputs
            : "a" (value), "d" (port));
}
