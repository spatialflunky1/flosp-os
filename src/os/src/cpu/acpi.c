#include <cpu/acpi.h>

xsdp_t* XSDP = NULL;
xsdt_t* XSDT = NULL;
fadt_t* FADT = NULL;
ui16_t num_XSDT_entries = 0;

void* get_acpi_table(char* signature) {
    for (ui16_t i = 0; i < num_XSDT_entries; i++) {
        char* header_signature = ((acpi_sdt_header_t*)XSDT->sdt_ptrs[i])->signature;
        if (signature[0] == header_signature[0] &&
            signature[1] == header_signature[1] &&
            signature[2] == header_signature[2] &&
            signature[3] == header_signature[3]) 
        {
            return (void*)XSDT->sdt_ptrs[i];
        }
    }
    return NULL;
}

void acpi_init(xsdp_t* XSDPTable) {
    XSDP = XSDPTable;
    XSDT = (xsdt_t*)XSDP->xsdt_ptr;
    num_XSDT_entries = (XSDT->sdt_header.length - sizeof(XSDT->sdt_header)) / 8;
    FADT = get_acpi_table("FACP");
    kern_log(FILTER_DEBUG, "Debug: ACPI table FADT (sign: \"FACP\") successfully located");
    if (FADT == NULL) {
        kern_log(FILTER_CRITICAL, "Fatal: ACPI table FADT not found");
        cpu_freeze();
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
    if (FADT->sdt_header.revision < 2 || (ui8_t)(FADT->IAPC_BOOT_ARCH & 0x0002) == 1) {
        // Assume PS/2 exists if table rev is under 2
        kern_log(FILTER_INFO, "PS/2 controller detected");
        return true;
    }
    kern_log(FILTER_INFO, "No PS/2 controller detected");
    return false;
}
