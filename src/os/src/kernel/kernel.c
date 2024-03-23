#include <kernel/kernel.h>

void main(BOOT_INFO* boot_info) {
    blank_output(&boot_info->VideoModeInfo);
    //clear_output_8025();
    //disable_cursor();
    //kern_log(FILTER_INFO, "Kernel flosp-"KVER" loaded");
    //idt_init();
    //kern_log(FILTER_INFO, "Interrupts enabled");
    //enable_lapic();
    //__asm__ volatile ("int $33");
    // Halt execution
    while (1) { 
        halt();
        __asm__ volatile ("cli; hlt");
    }
    return;
}
