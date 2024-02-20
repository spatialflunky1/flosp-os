#include <kernel/kernel.h>

int main(void) {
    clear_output_8025();
    //disable_cursor();
    #if MESSAGE_FILTER < FILTER_INFO
        kprint_8025("Kernel flosp-"KVER" loaded\n");
    #endif
    idt_init();
    #if MESSAGE_FILTER < FILTER_INFO
        kprint_8025("Interrupts enabled\n");
    #endif
    // Halt execution
    while(1) {
        halt();    
    }
    return 1; // In theory the kernel should never return, so if it does it will on error
}
