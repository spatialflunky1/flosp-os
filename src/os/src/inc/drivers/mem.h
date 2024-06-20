#ifndef MEM_H
#define MEM_H

#include <datatypes.h>

// Paging flags
// #define PTFlag_Present            1 << 0
// #define PTFlag_ReadWrite          1 << 1
// #define PTFlag_UserSuper          1 << 2
// #define PTFlag_WriteThrough       1 << 3
// #define PTFlag_CacheDisabled      1 << 4
// #define PTFlag_Accessed           1 << 5
// #define PTFlag_Dirty              1 << 6
// #define PTFlag_LargerPages        1 << 7
// #define PTFlag_Global             1 << 8
// #define PTFlag_Lensor_CopyOnWrite 1 << 9
// #define PTFlag_NX                 1 << 63

extern void flush_tlb(void);

void* memcpy(void* sourcep, void* destp, ui32_t nbytes);
void* memset(void *destp, int constant, ui32_t nbytes);
// int   mapmem(void* phys_addr, void* virt_addr, ui32_t flags);
// void* ioremap(void* phys_addr, ui64_t size, ui32_t flags);

#endif
