#ifndef GDT_H
#define GDT_H

extern void* gdt64_descriptor;

void gdt_init(void);

#endif
