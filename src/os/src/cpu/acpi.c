#include <cpu/acpi.h>

acpi_tables_t acpi_tables;
ui16_t num_XSDT_entries  = 0;
ui32_t lapic_x86_addr    = 0;
void* lapic_x86_64_addr  = NULL;

void* get_acpi_table(char* signature) {
    for (ui16_t i = 0; i < num_XSDT_entries; i++) {
        char* header_signature = ((acpi_sdt_header_t*)acpi_tables.XSDT->sdt_ptrs[i])->signature;
        if (signature[0] == header_signature[0] &&
            signature[1] == header_signature[1] &&
            signature[2] == header_signature[2] &&
            signature[3] == header_signature[3]) 
        {
            return (void*)acpi_tables.XSDT->sdt_ptrs[i];
        }
    }
    return NULL;
}

void acpi_init(xsdp_t* XSDPTable) {
    acpi_tables.XSDP = XSDPTable;
    acpi_tables.XSDT = (xsdt_t*)acpi_tables.XSDP->xsdt_ptr;
    acpi_tables.FADT = NULL;
    acpi_tables.HPET = NULL;
    acpi_tables.MADT = NULL;
    num_XSDT_entries = (acpi_tables.XSDT->sdt_header.length - sizeof(acpi_tables.XSDT->sdt_header)) / 8;
    // FADT
    acpi_tables.FADT = get_acpi_table("FACP"); 
    if (acpi_tables.FADT == NULL) {
        kern_log(FILTER_CRITICAL, "Fatal: ACPI table FADT not found");
        cpu_freeze();
    }
    kern_log(FILTER_DEBUG, "Debug: ACPI table FADT (sign: \"FACP\") successfully located");
    // HPET
    acpi_tables.HPET = get_acpi_table("HPET");
    if (acpi_tables.HPET == NULL) {
        kern_log(FILTER_CRITICAL, "Warning: ACPI table HPET not found");
    }
    else {
        kern_log(FILTER_DEBUG, "Debug: ACPI table HPET (sign: \"HPET\") successfully located");
    }
    // MADT
    acpi_tables.MADT = get_acpi_table("APIC");
    if (acpi_tables.MADT == NULL) {
        kern_log(FILTER_CRITICAL, "Warning: ACPI table MADT not found");
        cpu_freeze();
    }
    else {
        kern_log(FILTER_DEBUG, "Debug: ACPI table MADT (sign: \"ACPI\") successfully located");
    }
    // LAPIC address save
    lapic_x86_64_addr = get_madt_entry_addr(MADT_ENTRY_TYPE_LAPIC_ADDRESS_OVERRIDE);
    if (lapic_x86_64_addr == NULL) {
        lapic_x86_addr = acpi_tables.MADT->lapic_address;
    }
    else {
        // Move to offset with address (was at beginning of entry)
        lapic_x86_64_addr += 4;
    }
}

void acpi_vcheck(xsdp_t* XSDPTable) {
    if (XSDPTable->revision == 0) {
        kern_log(FILTER_CRITICAL, "Fatal: ACPI version 1.0 is not supported");
        cpu_freeze();
    }
    kern_log(FILTER_DEBUG, "Debug: ACPI version supported");
}

void acpi_ccheck(xsdp_t* XSDPTable) {
    ui64_t sum = 0;
    ui8_t* byte_array = (ui8_t*)XSDPTable;

    for (ui16_t i = 0; i < sizeof(xsdp_t); i++) {
        sum += byte_array[i];
    }

    if ((sum & 0xFF) != 0) {
        kern_log(FILTER_CRITICAL, "Fatal: ACPI checksum failed");
        cpu_freeze();
    }
    
    kern_log(FILTER_DEBUG, "Debug: ACPI passed checksum");
}

bool check_ps2(void) {
    if (acpi_tables.FADT->sdt_header.revision >= 2) { 
        kprint("Debug: FADT IAPC_BOOT_ARCH: ");
        kprint_bin(acpi_tables.FADT->IAPC_BOOT_ARCH, sizeof(acpi_tables.FADT->IAPC_BOOT_ARCH));
        kputchar('\n');
    }
    if (acpi_tables.FADT->sdt_header.revision < 2 || acpi_tables.FADT->IAPC_BOOT_ARCH & 2) {
        // Assume PS/2 exists if table rev is under 2
        kern_log(FILTER_INFO, "PS/2 controller detected");
        return true;
    }
    kern_log(FILTER_INFO, "No PS/2 controller detected");
    return false;
}

acpi_tables_t* get_acpi_tables(void) {
    return &acpi_tables;
}

void* get_madt_entry_addr(ui8_t entry_type) {
    ui8_t current_entry_type;
    ui8_t record_length;
    // First entry address
    ui64_t current_addr = (ui64_t)acpi_tables.MADT + 0x2C;
    while (current_addr <= (ui64_t)(acpi_tables.MADT + acpi_tables.MADT->sdt_header.length)) {
        current_entry_type = *(ui8_t*)(current_addr);
        record_length      = *(ui8_t*)(current_addr+1);
        if (current_entry_type == entry_type) {
            return (void*)current_addr;
        }
        if (record_length == 0) {
            return NULL;
        }
        current_addr += record_length;
    }
    return NULL;
}

void* get_lapic_address_acpi(void) {
    if (lapic_x86_64_addr == NULL) {
        return (void*)(ui64_t)lapic_x86_addr;
    }
    return lapic_x86_64_addr;
}
