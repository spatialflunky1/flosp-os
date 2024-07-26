#include <cpu/apic.h>

// LAPIC address
volatile void* lapic_addr = NULL;

ui8_t validate_rsdp(void* rsdp_ptr, size_t size) {
    char* rsdp = (char*)rsdp_ptr;
    ui32_t sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += rsdp[i];
    }
    return (sum & 0xFF) == 0;
}

void disable_pic8259(void) {
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
    ui32_t low = (base & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE;
    ui32_t high = (base >> 32) & 0x0f;
    wrmsr(IA32_APIC_BASE_MSR, low, high);
}

ui64_t lapic_get_base(void) {
    ui32_t low;
    ui32_t high;
    rdmsr(IA32_APIC_BASE_MSR, &low, &high);
    return (low & 0xfffff000) | ((ui64_t)(high & 0x0f) << 32);
}

void check_apic(void) {
    ui32_t edx = cpuid(1) >> 32;
    if ((edx & CPUID_FEAT_EDX_APIC) == 0) {
        kern_log(FILTER_CRITICAL, "APIC Not Supported");
        cpu_freeze();
    }
}

void enable_lapic(void) {
    kern_log(FILTER_DEBUG, "Debug: Checking for APIC support");
    check_apic();

    kern_log(FILTER_DEBUG, "Debug: Disabling PIC8259 and masking its interrupts");
    disable_pic8259();

    lapic_addr = get_lapic_address_acpi();
    kprint("Debug: Found LAPIC address at: ");
    kprint_hex((ui64_t)lapic_addr, true);
    kputchar('\n');

    kern_log(FILTER_DEBUG, "Debug: Enabling the LAPIC if it was not already done");
    lapic_set_base(lapic_get_base());

    kern_log(FILTER_DEBUG, "Debug: Setting the Spurious bit to start recieving interrupts");
    lapic_write_reg((ui64_t)lapic_addr + SPURIOUS_VECTOR, lapic_read_reg((ui64_t)lapic_addr + SPURIOUS_VECTOR) | 0x100); // Set bit 8, enable local APIC  
}

inline volatile void* get_lapic_address(void) {
    return lapic_addr;
}