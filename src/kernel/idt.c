#ifndef IDT_H
#include "idt.h"
#endif

void exception_handler() {
    __asm__ volatile ("cli; hlt");
}
