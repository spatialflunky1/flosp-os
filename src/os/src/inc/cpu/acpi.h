#ifndef ACPI_H
#define ACPI_H

#include <datatypes.h>
#include <kernel/message.h>
#include <kernel/power_mgmt.h>
#include <drivers/io.h>

#define MADT_ENTRY_TYPE_PROCESSOR_LAPIC              0x00
#define MADT_ENTRY_TYPE_IO_APIC                      0x01
#define MADT_ENTRY_TYPE_IO_APIC_INT_SRC_OVERRIDE     0x02
#define MADT_ENTRY_TYPE_IO_APIC_NON_MASKABLE_INT_SRC 0x03
#define MADT_ENTRY_TYPE_LAPIC_NON_MASKABLE_INT_SRC   0x04
#define MADT_ENTRY_TYPE_LAPIC_ADDRESS_OVERRIDE       0x05
#define MADT_ENTRY_TYPE_IO_SAPIC                     0x06
#define MADT_ENTRY_TYPE_LSAPIC                       0x07
#define MADT_ENTRY_TYPE_PLATFORM_INT_SRC             0x08
#define MADT_ENTRY_TYPE_PROCESSOR_Lx2APIC            0x09
#define MADT_ENTRY_TYPE_Lx2APIC_NMI                  0x0A
#define MADT_ENTRY_TYPE_GIC_CPU_INTERFACE            0x0B
#define MADT_ENTRY_TYPE_GIC_DISTRIBUTOR              0x0C
#define MADT_ENTRY_TYPE_GIC_MSI_FRAME                0x0D
#define MADT_ENTRY_TYPE_GIC_REDISTRIBUTOR            0x0E
#define MADT_ENTRY_TYPE_GIC_INT_TRANSLATION_SERVICE  0x0F
#define MADT_ENTRY_TYPE_MULTIPROCESSOR_WAKEUP        0x10
#define MADT_ENTRY_TYPE_CORE_PIC                     0x11
#define MADT_ENTRY_TYPE_LEGACY_IOPIC                 0x12
#define MADT_ENTRY_TYPE_HYPERTRANSPORT_PIC           0x13
#define MADT_ENTRY_TYPE_EXTENDED_IOPIC               0x14
#define MADT_ENTRY_TYPE_MSI_PIC                      0x15
#define MADT_ENTRY_TYPE_BRIDGE_IOPIC                 0x16
#define MADT_ENTRY_TYPE_LOW_PIN_COUNT_PIC            0x17

#define ACPI_MEMORY_SPACE 0

typedef struct {
    ui8_t  AddressSpace;
    ui8_t  BitWidth;
    ui8_t  BitOffset;
    ui8_t  AccessSize;
    ui64_t Address;
} __attribute__((packed)) GenericAddressStructure;

typedef struct {
    char   signature[8]; // 8 byte string ("RSD PTR ") not null terminated
    ui8_t  checksum;     // This value added to all other bytes of the table needs to equal 0
    char   oem_id[6];    // OEM identifier
    ui8_t  revision;     // Revision number
    ui32_t rsdt_ptr;     // Pointer to the RSDT
    // eXtended
    ui32_t len;          // Length of all SDTs (RSDT/XSDT)
    ui64_t xsdt_ptr;     // Pointer to the XSDT, if non-zero rsdt_ptr is ignored
    ui8_t  checksum_ext; // Extended checksum
    ui8_t  reserved[3];
} __attribute__((packed)) xsdp_t;

typedef struct {
    char   signature[4]; // 4 byte string ("XSDT") not null terminated
    ui32_t length;
    ui8_t  revision;
    ui8_t  checksum;
    char   oem_id[6];
    char   oem_table_id[8];
    ui32_t oem_revision;
    ui32_t creator_id;
    ui32_t creator_revision;
} __attribute__((packed)) acpi_sdt_header_t;

typedef struct {
    acpi_sdt_header_t sdt_header;
    ui64_t            sdt_ptrs[];
} __attribute__((packed)) xsdt_t;

typedef struct {
    acpi_sdt_header_t       sdt_header;
    ui32_t                  FIRMWARE_CTRL;
    ui32_t                  DSDT;
    ui8_t                   Reserved0;
    ui8_t                   Preferred_PM_Profile;
    ui16_t                  SCI_INT;
    ui32_t                  SMI_CMD;
    ui8_t                   ACPI_ENABLE;
    ui8_t                   ACPI_DISABLE;
    ui8_t                   S4BIOS_REQ;
    ui8_t                   PSTATE_CNT;
    ui32_t                  PM1a_EVT_BLK;
    ui32_t                  PM1b_EVT_BLK;
    ui32_t                  PM1a_CNT_BLK;
    ui32_t                  PM1b_CNT_BLK;
    ui32_t                  PM2_CNT_BLK;
    ui32_t                  PM_TMR_BLK;
    ui32_t                  GPE0_BLK;
    ui32_t                  GPE1_BLK;
    ui8_t                   PM1_EVT_LEN;
    ui8_t                   PM1_CNT_LEN;
    ui8_t                   PM2_CNT_LEN;
    ui8_t                   PM_TMR_LEN;
    ui8_t                   GPE0_BLK_LEN;
    ui8_t                   GPE1_BLK_LEN;
    ui8_t                   GPE1_BASE;
    ui8_t                   CST_CNT;
    ui16_t                  P_LVL2_LAT;
    ui16_t                  P_LVL3_LAT;
    ui16_t                  FLUSH_SIZE;
    ui16_t                  FLUSH_STRIDE;
    ui8_t                   DUTY_OFFSET;
    ui8_t                   DUTY_WIDTH;
    ui8_t                   DAY_ALRM;
    ui8_t                   MON_ALRM;
    ui8_t                   CENTURY;
    ui16_t                  IAPC_BOOT_ARCH;
    ui8_t                   Reserved1;
    ui32_t                  Flags;
    GenericAddressStructure RESET_REG;
    ui8_t                   RESET_VALUE;
    ui16_t                  ARM_BOOT_ARCH;
    ui8_t                   FADT_Minor_Version;
    ui64_t                  X_FIRMWARE_CTRL;
    ui64_t                  X_DSDT;
    GenericAddressStructure X_PM1a_EVT_BLK;
    GenericAddressStructure X_PM1b_EVT_BLK;
    GenericAddressStructure X_PM1a_CNT_BLK;
    GenericAddressStructure X_PM1b_CNT_BLK;
    GenericAddressStructure X_PM2_CNT_BLK;
    GenericAddressStructure X_PM_TMR_BLK;
    GenericAddressStructure X_GPE0_BLK;
    GenericAddressStructure X_GPE1_BLK;
    GenericAddressStructure SLEEP_CONTROL_REG;
    GenericAddressStructure SLEEP_STATUS_REG;
    ui64_t                  Hypervisor_Vendor_Identity;
} __attribute__((packed)) fadt_t;

typedef struct {
    ui8_t  Hardware_Rev_ID;
    ui8_t  Num_Comparators:5;
    ui8_t  Counter_Size:1;
    ui8_t  Reserved:1;
    ui8_t  Legacy_Replacement:1;
    ui16_t PCI_Vendor_ID;
} __attribute__((packed)) event_timer_block_id_t;

typedef struct {
    acpi_sdt_header_t       sdt_header;
    event_timer_block_id_t  Event_Timer_Block_ID;
    GenericAddressStructure Base_Address;
    ui8_t                   HPET_Num;
    ui16_t                  Minimum_Clock_Tick;
    ui8_t                   Page_Protection;
} __attribute__((packed)) hpet_t;

typedef struct {
    acpi_sdt_header_t sdt_header;
    ui32_t            lapic_address;
    ui32_t            flags;
} __attribute__((packed)) madt_t;

typedef struct {
    xsdp_t* XSDP;
    xsdt_t* XSDT;
    fadt_t* FADT;
    hpet_t* HPET;
    madt_t* MADT;
} acpi_tables_t;

void  acpi_init(xsdp_t* XSDPTable);
void* get_acpi_table(char* signature);
void  acpi_vcheck(xsdp_t* XSDPTable); // Version check
void  acpi_ccheck(xsdp_t* XSDPTable); // Checksum check
bool  check_ps2(void);                // Check if a PS/2 controller is connected
acpi_tables_t* get_acpi_tables(void);
void* get_madt_entry_addr(ui8_t entry_type);
void* get_lapic_address_acpi(void);

#endif
