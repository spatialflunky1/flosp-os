#ifndef MEM_H
#define MEM_H

#include <datatypes.h>
// (for paging)
#include <cpu/cpu.h>

// Paging flags
#define PFlag_Present            1 << 0
#define PFlag_ReadWrite          1 << 1
#define PFlag_UserSuper          1 << 2
#define PFlag_PWT                1 << 3
#define PFlag_PCD                1 << 4
#define PFlag_Accessed           1 << 5
#define PFlag_Dirty              1 << 6
#define PFlag_LargerPages        1 << 7
#define PFlag_Global             1 << 8
#define PFlag_PAT                1 << 12
#define PFlag_NX                 1 << 63
// Bit mask for aligning address to 52 bits
#define PAGE_ADDR_MASK 0x000ffffffffff000

typedef int word; // "optimal size of data transfer for performance"

extern void flush_tlb(void);

typedef struct {
    ui64_t entries[512];
} map_table;

// Assembly function abstraction
static inline void invlpg(void* addr) {
    __asm__ volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

void* memcpy(void* sourcep, void* destp, ui32_t nbytes);
void* memset(void *destp, int constant, ui32_t nbytes);
void set_mem_page_flags(void* addr, ui64_t flags);
void* get_mem_page(void* virt_addr);
// int   mapmem(void* phys_addr, void* virt_addr, ui32_t flags);
// void* ioremap(void* phys_addr, ui64_t size, ui32_t flags);

#endif
