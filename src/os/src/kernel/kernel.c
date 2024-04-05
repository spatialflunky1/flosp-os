#include <kernel/kernel.h>

void main(BOOT_INFO* boot_info) {
    // Video
    initialize_video(&boot_info->VideoModeInfo);
    kern_log(FILTER_INFO, "Kernel flosp-"KVER" loaded");
    // GDT
    gdt_init();
    kern_log(FILTER_INFO, "GDT initialized");
    
    // IDT
    idt_init();
    kern_log(FILTER_INFO, "IDT initialized and interrupts enabled");
    // ACPI version check
    acpi_vcheck(boot_info->XSDPTable);
    // ACPI Checksum check
    acpi_ccheck(boot_info->XSDPTable);
    // LAPIC
    enable_lapic();
    kern_log(FILTER_INFO, "LAPIC enabled");
    // Initialize keyboard
    keyboard_init();
    kern_log(FILTER_INFO, "Keyboard initialized");
    // Halt execution
    while (true) { 
        __asm__ volatile ("hlt");
    }
    return;
}
