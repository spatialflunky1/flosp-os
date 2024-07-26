#include "datatypes.h"
#include "drivers/vga.h"
#include <drivers/mem.h>

void* memcpy(void* sourcep, void* destp, ui32_t nbytes) {
    /*
     * When both pointers and number of bytes are word aligned, copy "word-at-a-time" instead of "byte-at-a-time"... Wise words I see
     */

    // "word-at-a-time"
    if ((ui64_t)destp % sizeof(word) == 0 &&
        (ui64_t)sourcep % sizeof(word) == 0 &&
        nbytes % sizeof(word) == 0) 
        {
            const word* source = (word*)sourcep;
            word*       dest   = (word*)destp;
            for (ui32_t i = 0; i < nbytes/sizeof(word); i++) {
                dest[i] = source[i];
            }
    }
    // "byte-at-a-time"
    else {
        const ui8_t* source = (ui8_t*)sourcep;
        ui8_t*       dest   = (ui8_t*)destp;
        for (ui32_t i = 0; i < nbytes; i++) {
            dest[i] = source[i];
        }
    }
    return destp;
}

void* memset(void *destp, int constant, ui32_t nbytes) {
    ui8_t* dest = (ui8_t*)destp;
    for (ui32_t i = 0; i < nbytes; i++) {
        dest[i] = constant;
    }
    return destp;
}

//
// PAGGING FUNCTIONS FOR A LATER USE
//

void set_mem_page_flags(void* addr, ui64_t flags) {
    ui64_t* page = (ui64_t*)get_mem_page(addr);

    blank_output();
    // Address
    kprint("\nAddress:    ");
    kprint_hex((ui64_t)addr, true);
    // Page address
    kprint("\nPage_Addr:  ");
    kprint_hex((ui64_t)page, true);
    // Page
    kprint("\nPage:       ");
    kprint_hex(*page, true);
    // Flags
    kprint("\nFlags:      ");
    kprint_hex(flags, true);
    flush_buffer();
    
    // // Remove previous mapping
    // (*page) &= ~0x1;
    // // Flush the TLB
    // invlpg(addr);
    // // Create a new mapping in the page tables
    (*page) |= flags;
    // (*page) |= 0x1;
    // kprint("\nMapping created");
    // flush_buffer();

    kprint("\nPage(Post): ");
    kprint_hex((*page), true);
    flush_buffer();

    blank_output();
    flush_buffer();
}

void* get_mem_page(void* addr) {
    // Indexing bits:
    // ---------------
    // 47-39: PML4 (9 bits)
    // 38-30: PDPT (9 bits)
    // 29-21: PD   (9 bits)
    // 20-12: PT   (9 bits)
    // 0-11:  The page itself (12 bits)
    map_table* PML4 = (map_table*)(read_cr3() & 0xFFFFFFFFFFFFF000); // First 12 bits are to be ignored
    ui64_t PML4_index = ((ui64_t)addr >> 39) & 0b111111111;

    map_table* PDP = (map_table*)(PML4->entries[PML4_index] & PAGE_ADDR_MASK);
    ui64_t PDP_index = ((ui64_t)addr >> 30) & 0b111111111;
    
    void* page = NULL;
    //ui8_t  page_offset_width = 0;
    if ((PDP->entries[PDP_index] >> 7) & 0x1) {
        // Pages are 1 GiB (ignore PD)
        page = &PDP->entries[PDP_index];
        //page_offset_width = 30;
    }
    else {
        map_table* PD = (map_table*)(PDP->entries[PDP_index] & PAGE_ADDR_MASK);
        ui64_t PD_index = ((ui64_t)addr >> 21) & 0b111111111;

        if ((PD->entries[PD_index] >> 7) & 0x1) {
            // Pages are 2 Mib (ignore PT)
            page = &PD->entries[PD_index];
            //page_offset_width = 21;
        }
        else {
            // Pages are 4 Kib (use PT)
            map_table* PT = (map_table*)(PD->entries[PD_index] & PAGE_ADDR_MASK);
            ui64_t PT_index = ((ui64_t)addr >> 12) & 0b111111111;

            page = &PT->entries[PT_index];
            //page_offset_width = 12;
        }
    }

    // ui64_t phys_addr_index = (ui64_t)addr & (1<<page_offset_width) - 1;
    // kprint("\nPhysAddrI:  ");
    // kprint_hex(phys_addr_index, true);
    // ui64_t phys_addr = (page & ~0xFFF) + phys_addr_index;
    // kprint("\nPhysAddr:   ");
    // kprint_hex(phys_addr, true);
    return page;
}

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