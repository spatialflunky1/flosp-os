#include <kernel/power_mgmt.h>

void halt(void) {
    kern_log(FILTER_CRITICAL, "CPU Halted");
    //set_cursor_pos(curs_loc);
    __asm__ volatile ("cli; hlt");
}
