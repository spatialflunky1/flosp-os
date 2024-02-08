#include "kernel.h"

// Global Vars 

void main(void) {
    reset_output();
    disable_cursor();
    print("Kernel flosp-");
    print(KVER);
    print(" loaded\n\0");
    for(;;);
}
