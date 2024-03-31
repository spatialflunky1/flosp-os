#ifndef GDT_H
#define GDT_H

#include <kernel/message.h>

extern void* gdt64_descriptor;

void gdt_init(void);

#endif
