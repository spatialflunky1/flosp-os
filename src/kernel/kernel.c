#include "kernel.h"

// Global Vars 

void main(void) {
    reset_output();
    disable_cursor();
    kprint_8024("Kernel flosp-");
    kprint_8024(KVER);
    kprint_8024(" loaded\n\0");
    for(;;);
}
