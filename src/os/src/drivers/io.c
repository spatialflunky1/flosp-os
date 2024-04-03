#include <drivers/io.h>

void outb(ui16_t port, ui8_t value) {
    __asm__("out %%al,%%dx"
            : // No outputs
            : "a" (value), "d" (port));
}

ui8_t inb(ui16_t port) {
    ui8_t value;
    __asm__ ("in %%dx,%%al"
            : "=a"(value)
            : "d"(port));
    return value;
}
