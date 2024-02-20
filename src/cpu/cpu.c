#include <cpu/cpu.h>

void halt() {
    kprint_8025("CPU Halted");
    __asm__ volatile ("cli; hlt");
}
