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
    // ACPI pointer check
    if (boot_info->XSDPTable == NULL) {
        kern_log(FILTER_CRITICAL, "Fatal: Invalid ACPI XSDP address");
        cpu_freeze();
    }
    // ACPI version check
    acpi_vcheck(boot_info->XSDPTable);
    // ACPI Checksum check
    acpi_ccheck(boot_info->XSDPTable);
    // ACPI Init (variables and such)
    acpi_init(boot_info->XSDPTable);
    kern_log(FILTER_INFO, "ACPI enabled");
    // LAPIC
    enable_lapic();
    kern_log(FILTER_INFO, "LAPIC enabled");
    // Timers
    kern_log(FILTER_DEBUG, "Debug: Initializing the system timer for kernel usage");
    timers_init();
    // PS/2
    // if (check_ps2()) {
    //     ps2_init();
    // }
    // // USB

    // // Initialize keyboard
    // keyboard_init();
    kscroll_down();
    // Start system prompt
    enter_sysprompt();
    // Halt execution
    while (true) { 
        __asm__ volatile ("hlt");
    }
    return;
}
