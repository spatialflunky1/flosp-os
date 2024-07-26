#include <cpu/idt.h>

void page_fault_handler(cpu_status_t* cpu_status) {
    // Remove the or 1 when paging is to be started
    if (cpu_status->error_code & 0x01 || 1) {
        // Page is present
        // Panic
        kern_log(FILTER_CRITICAL, "\nFlosp OS has encountered an exception and cannot continue");
        kern_log(FILTER_CRITICAL, "Page Fault");
        kprint("Error Code: ");
        kprint_hex(cpu_status->error_code, 1);
        kprint("\n");
        cpu_freeze();
    }
    // Page is not present
    // We can recover from this (later)
    //ui64_t virt_addr = read_cr2();
}

cpu_status_t* interrupt_handler(cpu_status_t* cpu_status) {
    if (cpu_status->error) {
        if (cpu_status->int_vector == 0x0E) {
            page_fault_handler(cpu_status);
            return cpu_status;
        }
        kern_log(FILTER_CRITICAL, "\nFlosp OS has encountered an exception and cannot continue");
        switch (cpu_status->int_vector) {
            case 0x0D:
                kern_log(FILTER_CRITICAL, "General Protection Fault");
                break;
            default:
                kprint("Interrupt vector: ");
                kprint_hex(cpu_status->int_vector, 1);
                kprint("\n");
                break;
        }
        kprint("Error Code: ");
        kprint_hex(cpu_status->error_code, 1);
        kprint("\n");
        cpu_freeze();
    }
    // Reach here on non-error interrupts
    switch (cpu_status->int_vector) {
        case 0x20:
            // System Timer Interrupt
            cpu_status = timer_handler(cpu_status);
            break;
        case 0x21:
            // Keyboard interrupt
            if (check_keyboard_enabled()) {
                keyboard_handler();
            }
            break;
        case 0x2C:
            // Mouse interrupt
            flush_ps2_output_buffer();
            // if (check_mouse_enabled()) {
            //     mouse_handler();
            // }
            break;
        default:
            kprint("Unexpected Interrupt: ");
            kprint_hex(cpu_status->int_vector, 1);
            kprint("\n");
            break;
    }
    // Send EOI to PIC
    // Also send EOI to slave pic if vector is at or above IRQ 8
    if (cpu_status->int_vector >= 8+32) {
        outb(PIC_COMMAND_SLAVE, PIC_EOI);
    }
    outb(PIC_COMMAND_MASTER, PIC_EOI);
    return cpu_status;
}

void idt_set_descriptor(ui8_t vect, void* isr, ui8_t flags) {
    idt_entry_t* descriptor = &idt[vect];
    ui64_t isr_addr = (ui64_t)isr;
    descriptor->isr_low    = isr_addr & 0xFFFF;
    descriptor->kernel_cs  = 0x08;
    descriptor->ist        = 0;
    descriptor->attributes = flags; 
    descriptor->isr_mid    = (isr_addr >> 16) & 0xFFFF;
    descriptor->isr_high   = (isr_addr >> 32) & 0xFFFFFFFF;
    descriptor->reserved   = 0;
}

void mask_pic_interrupts(void) {
    outb(PIC_DATA_MASTER, 0xFF);
    outb(PIC_DATA_SLAVE, 0xFF);
}

void idt_init(void) {
    kern_log(FILTER_DEBUG, "Debug: Defining and setting IDT entries");
    idtr.base = &idt[0];
    idtr.limit = sizeof(idt_entry_t) * MAX_IDT_ENTRIES - 1;
    // Set reserved interrupts
    for (ui8_t vect = 0; vect < 32; vect++) {
        idt_set_descriptor(vect, isr_stub_table[vect], 0x8E); // 1 00 0(8) 1110(E)
    }
    // Set IRQs
    for (ui8_t vect = 32; vect < 32+16; vect++) {
        idt_set_descriptor(vect, isr_stub_table[vect], 0x8E);
    }
    kern_log(FILTER_DEBUG, "Debug: Masking PIC interrupts");
    mask_pic_interrupts();
    kern_log(FILTER_DEBUG, "Debug: Loading IDT");
    __asm__ volatile ("lidt %0" :: "m"(idtr));
    kern_log(FILTER_DEBUG, "Debug: Setting interrupt flag");
    __asm__ volatile ("sti");
}
