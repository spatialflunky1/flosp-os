#ifndef ACPI_H
#define ACPI_H

#include <kernel/datatypes.h>
#include <kernel/message.h>
#include <kernel/power_mgmt.h>

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

void acpi_vcheck(xsdp_t* XSDPTable); // Version check
void acpi_ccheck(xsdp_t* XSDPTable); // Checksum check

#endif
