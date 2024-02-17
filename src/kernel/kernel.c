#include "kernel.h"

// Global Vars 

int main(void) {
    clear_output_8025();
    //disable_cursor();
    kprint_8025("Kernel flosp-"KVER" loaded\n");
    idt_init();
    // Halt execution
    while(1) {
        __asm__ volatile ("cli; hlt");
    }
    return 1; // In theory the kernel should never return, so if it does it will on error
}
