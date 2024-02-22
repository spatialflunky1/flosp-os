#include <kernel/kernel.h>

int main(void) {
    clear_output_8025();
    //disable_cursor();
    kern_log(FILTER_INFO, "Kernel flosp-"KVER" loaded");
    idt_init();
    kern_log(FILTER_INFO, "Interrupts enabled");
    // Halt execution
    while(1) {
        halt();
        //__asm__ volatile ("hlt");
    }
    return 1; // In theory the kernel should never return, so if it does it will on error
}
