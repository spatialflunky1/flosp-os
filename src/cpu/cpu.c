#include <cpu/cpu.h>

void halt() {
    kprint_8025("CPU Halted\n");
    __asm__ volatile ("cli; hlt");
}
