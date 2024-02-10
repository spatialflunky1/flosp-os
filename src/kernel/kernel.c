#include "kernel.h"

// Global Vars 

void main(void) {
    reset_output_8024();
    disable_cursor();
    kprint_8024("Kernel flosp-"KVER" loaded\n");
    for(;;);
}
