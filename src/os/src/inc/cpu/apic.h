#ifndef ACPI_H
#define ACPI_H

#include <kernel/datatypes.h>
#include <drivers/io.h>
#include <cpu/cpu.h>
#include <kernel/message.h>

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

#define IA32_APIC_BASE_MSR 0x1B 
#define IA32_APIC_BASE_MSR_ENABLE 0x800

#define SPURIOUS_VECTOR 0xF0

extern ui32_t* apic_virt_base;

typedef struct {
    char   signature[8]; // 8 byte string ("RSD PTR ") not null terminated
    ui8_t  checksum;     // This value added to all other bytes of the table needs to equal 0
    char   oem_id[6];    // OEM identifier
    ui8_t  revision;     // Revision number
    ui32_t rsdt_ptr;     // Pointer to the RSDT
    ui32_t len;          // Length of all SDTs (RSDT/XSDT)
    ui64_t xsdt_ptr;     // Pointer to the XSDT, if non-zero rsdt_ptr is ignored
    ui8_t  checksum_ext; // Extended checksum
    ui8_t  reserved[3];
} __attribute__((packed)) rsdp_t;

typedef struct {
    char   signature[4]; // 4 byte string ("XSDT") not null terminated
    ui32_t len;
    ui8_t  rev;
    ui8_t  checksum;
    char   oem_id[6];
    char   oem_table_id[8];
    ui32_t oem_revision;
    ui32_t creator_id;
    ui32_t creator_revision;
} __attribute__((packed)) acpi_sdt_t;

typedef struct {
    acpi_sdt_t sdt_header;
    ui64_t sdt_ptrs[];
} __attribute__((packed)) xsdt_t;

static inline ui32_t lapic_read_reg(ui64_t reg) {
    return *(volatile ui32_t*)reg;
}

static inline void lapic_write_reg(ui64_t reg, ui32_t val) {
    *(volatile ui32_t*)reg = val;
}

ui8_t validate_rsdp(void* rsdp_ptr, size_t size); // The last byte of the sum of all bytes needs to equal 0 to pass validation
void disable_pic8259(void);
void lapic_set_base(ui64_t base);
ui64_t lapic_get_base();
void check_apic();
void enable_lapic(void);

#endif
