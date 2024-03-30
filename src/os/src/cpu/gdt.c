#include <cpu/gdt.h>

void gdt_init(void) {
    __asm__ volatile ("lgdt %0" :: "m"(gdt64_descriptor));
}
