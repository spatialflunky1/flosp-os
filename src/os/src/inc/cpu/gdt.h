#ifndef GDT_H
#define GDT_H

#include <kernel/message.h>

extern void gdt_init_asm(void);

void gdt_init(void);

#endif
