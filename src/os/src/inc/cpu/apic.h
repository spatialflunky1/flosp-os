#ifndef APIC_H
#define APIC_H

#include <datatypes.h>
#include <drivers/io.h>
#include <cpu/cpu.h>
#include <kernel/message.h>
#include <cpu/acpi.h>

#define PIC_COMMAND_MASTER 0x20
#define PIC_COMMAND_SLAVE  0xA0
#define PIC_DATA_MASTER    0x21
#define PIC_DATA_SLAVE     0xA1
// Initialization Command Words (bytes despite the fact they are called words)
#define ICW_1    0x11 // Indicates start of initialization sequence
                      // Last bit indicates ICW_4 is present
#define ICW_2_Ma 0x20 // Interrupt vector address offset (above the first 0-31)
#define ICW_2_Sl 0x28 // Slave PIC offset
#define ICW_3_Ma 0x02 // Indicates slave PIC at IRQ 2
#define ICW_3_Sl 0x04 // Slave cascade ID
#define ICW_4    0x01 // Specifies 8086 mode

#define IA32_APIC_BASE_MSR        0x1B 
#define IA32_APIC_BASE_MSR_ENABLE 0x800

#define SPURIOUS_VECTOR 0xF0

#define APIC_APICID	             0x20
#define APIC_APICVER             0x30
#define APIC_TASKPRIOR           0x80
#define APIC_EOI                 0x0B0
#define APIC_LDR	             0x0D0
#define APIC_DFR	             0x0E0
#define APIC_SPURIOUS            0x0F0
#define APIC_ESR	             0x280
#define APIC_ICRL	             0x300
#define APIC_ICRH	             0x310
#define APIC_LVT_TIMER           0x320
#define APIC_LVT_PERF	         0x340
#define APIC_LVT_LINT0	         0x350
#define APIC_LVT_LINT1	         0x360
#define APIC_LVT_ERR	         0x370
#define APIC_TIMER_INITIAL_COUNT 0x380
#define APIC_TIMER_CURRENT_COUNT 0x390
#define APIC_TIMER_DIVIDER       0x3E0
#define APIC_LAST	             0x38F
#define APIC_DISABLE	         0x10000
#define APIC_SW_ENABLE         	 0x100
#define APIC_CPUFOCUS	         0x200
#define APIC_NMI	             4<<8
#define APIC_TIMER_MODE_PERIODIC 0x20000
#define APIC_TIMER_BASEDIV       1<<20

static inline ui32_t lapic_read_reg(ui64_t reg) {
    return *(volatile ui32_t*)reg;
}

static inline void lapic_write_reg(ui64_t reg, ui32_t val) {
    *(volatile ui32_t*)reg = val;
}

ui8_t          validate_rsdp(void* rsdp_ptr, size_t size); // The last byte of the sum of all bytes needs to equal 0 to pass validation
void           disable_pic8259(void);
void           lapic_set_base(ui64_t base);
ui64_t         lapic_get_base(void);
void           check_apic(void);
void           enable_lapic(void);
volatile void* get_lapic_address(void);
#endif
