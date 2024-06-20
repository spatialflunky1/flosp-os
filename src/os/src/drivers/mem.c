#include <drivers/mem.h>

void* memcpy(void* sourcep, void* destp, ui32_t nbytes) {
    ui8_t* source = (ui8_t*)sourcep;
    ui8_t* dest   = (ui8_t*)destp;
    for (ui32_t i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
    return destp;
}

void* memset(void *destp, int constant, ui32_t nbytes) {
    ui8_t* dest = (ui8_t*)destp;
    for (ui32_t i = 0; i < nbytes; i++) {
        *(dest + i) = constant;
    }
    return destp;
}

//
// PAGGING FUNCTIONS FOR A LATER USE
//

// // 0: Success
// // 1: Error
// int mapmem(void* phys_addr, void* virt_addr, ui32_t flags) {
//     // Make sure both addresses are page aligned
//     if ((ui64_t)phys_addr & 0xFFF || (ui64_t)virt_addr & 0xFFF) {
//         return 1;
//     }
//     // Page Directory Index
//     ui64_t  pd_index = (ui64_t)virt_addr >> 22;
//     // Page Table Index
//     ui64_t  pt_index = (ui64_t)virt_addr >> 12 & 0x03FF;
    
//     ui64_t* pd = (ui64_t*)0xFFFFF000;
//     // Check if PD entry is present
//     if (!(pd[pd_index] & PTFlag_Present)) {
//         // No PD entry present
//         return 1;
//     }
    
//     ui64_t* pt = ((ui64_t*)0xFFC00000) + (0x400 * pd_index);
//     // Check if there is a present PT entry
//     /*
//     if (pt[pt_index] & PTFlag_Present) {
//         return 1;
//     }
//     */

//     // Set address mapping with specified flags
//     pt[pt_index] = ((ui64_t)phys_addr) | (flags & 0xFFF);

//     // Flush entry in the TLB
//     flush_tlb();
//     return 0;
// }

// void* ioremap(void* phys_addr, ui64_t size, ui32_t flags) {
//     for (ui64_t addr = (ui64_t)phys_addr; addr < (ui64_t)phys_addr + size; addr++) {
//         if (mapmem((void*)addr, (void*)addr, flags) == 1) {
//             return NULL;
//         }
//     }
//     return phys_addr;
// }