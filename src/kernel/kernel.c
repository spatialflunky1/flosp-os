#include "kernel.h"
#include <complex.h>

// Global Vars 

void main(void) {
    clear_output_8025();
    disable_cursor();
    kprint_8025("Kernel flosp-"KVER" loaded\n");
    // Halt execution
    __asm__ volatile ("cli; hlt");
}
