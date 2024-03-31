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
    // LAPIC
    enable_lapic();
    kern_log(FILTER_INFO, "APIC Enabled");
    //__asm__ volatile ("int $33");
    // Halt execution
    while (1) { 
        halt();
    }
    return;
}
