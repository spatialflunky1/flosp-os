#ifndef IDT_H
#include "idt.h"
#endif

void exception_handler() {
    __asm__ volatile ("cli; hlt");
}

void idt_set_descriptor(unsigned char vect, void* isr, unsigned char flags) {
   idt_entry_t* descriptor = &idt[vect];
   descriptor->isr_low    = (unsigned long long)isr & 0xFFFF;
   descriptor->kernel_cs  = 0x08;
   descriptor->ist        = 0;
   descriptor->attributes = flags; 
   descriptor->isr_mid    = ((unsigned long long)isr >> 16) & 0xFFFF;
   descriptor->isr_high   = ((unsigned long long)isr >> 32) & 0xFFFFFFFF;
   descriptor->reserved   = 0;
}

void idt_init() {
    idtr.base = (unsigned long long)&idt[0];
    idtr.limit = (unsigned short)sizeof(idt_entry_t) * 256 - 1; // 0xFFF: 16 bytes per descriptor * 256 descriptors -1
    for (unsigned char vect = 0; vect < 32; vect++) {
        idt_set_descriptor(vect, isr_stub_table[vect], 0x8E); // 1 00 0(8) 1110(E)
    }
    __asm__ volatile ("lidt %0" :: "m"(idtr));
    __asm__ volatile ("sti");
}
