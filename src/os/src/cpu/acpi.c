#include <cpu/acpi.h>

void acpi_vcheck(xsdp_t* XSDPTable) {
    if (XSDPTable->revision == 0) {
        kern_log(FILTER_CRITICAL, "Fatal: ACPI version 1.0 is not supported");
        cpu_freeze();
    }
    kern_log(FILTER_DEBUG, "ACPI version supported");
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
    
    kern_log(FILTER_DEBUG, "ACPI passed checksum");
}
