void outb(unsigned short port, unsigned char value) {
    __asm__("out %%al,%%dx"
            : // No outputs
            : "a" (value), "d" (port));
}
