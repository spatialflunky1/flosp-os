#include <cpu/idt.h>

cpu_status_t* exception_handler(cpu_status_t* cpu_status) {
    switch (cpu_status->int_vector) {
        case 13:
            kern_log(FILTER_CRITICAL, "General Protection Fault");
            break;
        case 14:
            kern_log(FILTER_CRITICAL, "Page Fault");
            break;
        default:
            kprint_8025("Unexpected Fault: ");
            kprint_hex_8025(cpu_status->int_vector);
            kprint_8025("\n");
            break;
    }
    if (cpu_status->error) {
        halt();
    }
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

void idt_init() {
    idtr.base = &idt[0];
    idtr.limit = sizeof(idt_entry_t) * MAX_IDT_ENTRIES - 1;
    for (ui8_t vect = 0; vect < 32; vect++) {
        idt_set_descriptor(vect, isr_stub_table[vect], 0x8E); // 1 00 0(8) 1110(E)
    }
    __asm__ volatile ("lidt %0" :: "m"(idtr));
    __asm__ volatile ("sti");
}
