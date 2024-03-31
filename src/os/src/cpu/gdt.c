#include <cpu/gdt.h>

void gdt_init(void) {
    kern_log(FILTER_DEBUG, "Loading GDT");
    __asm__ volatile ("lgdt %0" :: "m"(gdt64_descriptor));
}
