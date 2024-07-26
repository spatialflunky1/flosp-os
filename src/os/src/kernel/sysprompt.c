#include <kernel/sysprompt.h>

void enter_sysprompt(void) {
    kprint("C:\\> ");
    for (ui32_t i = 0; i < 1000; i++) {
        kprint_num(i);
        kputchar('\n');
    }
}
