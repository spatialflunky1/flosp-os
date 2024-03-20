#include <cpu/apic.h>

ui32_t* apic_virt_base;

ui8_t validate_rsdp(void* rsdp_ptr, size_t size) {
    char* rsdp = (char*)rsdp_ptr;
    ui32_t sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += rsdp[i];
    }
    return (sum & 0xFF) == 0;
}

void disable_pic8259() {
    outb(PIC_COMMAND_MASTER, ICW_1);
    outb(PIC_COMMAND_SLAVE, ICW_1);
    outb(PIC_DATA_MASTER, ICW_2_Ma);
    outb(PIC_DATA_SLAVE, ICW_2_Sl);
    outb(PIC_DATA_MASTER, ICW_3_Ma);
    outb(PIC_DATA_SLAVE, ICW_3_Sl);
    outb(PIC_DATA_MASTER, ICW_4);
    outb(PIC_DATA_SLAVE, ICW_4);
    // PIC Interrupts were already masked when enabling Interrupts 0-31
}

void lapic_set_base(ui64_t base) {
    wrmsr(IA32_APIC_BASE_MSR, 
          (base & 0xFFFFF0000) | IA32_APIC_BASE_MSR_ENABLE);
}

ui64_t lapic_get_base() {
    return rdmsr(IA32_APIC_BASE_MSR) & 0xFFFFFF000;
}

void check_apic() {
    ui32_t edx = cpuid(1) >> 32;
    if ((edx & CPUID_FEAT_EDX_APIC) == 0) {
        kern_log(FILTER_CRITICAL, "APIC Not Supported");
        halt();
    }
}

void enable_lapic() {
    check_apic();
    disable_pic8259();
    lapic_set_base(lapic_get_base());
    lapic_write_reg(SPURIOUS_VECTOR, lapic_read_reg(SPURIOUS_VECTOR) | 0x100); // Set bit 8, enable local APIC 
    kern_log(FILTER_INFO, "APIC Enabled");
}
