#include <drivers/vga.h>
#include <cpu/idt.h>
#include <kernel/message.h>
#include <kernel/power_mgmt.h>
#include <cpu/apic.h>

typedef struct {
    ui32_t Type;
    ui64_t PhysicalStart;
    ui64_t VirtualStart;
    ui64_t NumberOfPages;
    ui64_t Attribute;
} MEMORY_DESCRIPTOR;

typedef struct {
    MEMORY_DESCRIPTOR*   MemoryMap;
    ui64_t               MemoryMapSize;
    ui64_t               MemoryMapDescriptorSize;
    BOOT_VIDEO_MODE_INFO VideoModeInfo;
} BOOT_INFO;

#ifndef KVER
#define KVER "0.0.0"
#endif
