#ifndef MEM_H
#define MEM_H

#include <kernel/datatypes.h>

void* memcpy(void* sourcep, void* destp, ui32_t nbytes);
void* memset(void *destp, int constant, ui32_t nbytes);

#endif
