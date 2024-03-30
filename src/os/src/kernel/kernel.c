#include <kernel/kernel.h>

void main(BOOT_INFO* boot_info) {
    // Video
    initialize_video(&boot_info->VideoModeInfo);
    kern_log(FILTER_INFO, "Kernel flosp-"KVER" loaded");
    // GDT
    gdt_init();
    kern_log(FILTER_INFO, "GDT Initialized");
    // IDT
    idt_init();
    kern_log(FILTER_INFO, "IDT Initialized and Interrupts enabled");
    //enable_lapic();
    //__asm__ volatile ("int $13");
    // Halt execution
    while (1) { 
        halt();
        // Interrupts cleared halt only as a precaution
        __asm__ volatile ("cli; hlt");
    }
    return;
}
