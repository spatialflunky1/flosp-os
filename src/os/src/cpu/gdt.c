#include <cpu/gdt.h>

void gdt_init(void) {
    kern_log(FILTER_DEBUG, "Loading GDT");    
    gdt_init_asm();
}
