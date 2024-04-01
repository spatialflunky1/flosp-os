#include <kernel/power_mgmt.h>

void halt(void) {
    kern_log(FILTER_CRITICAL, "CPU Halted");
    __asm__ volatile ("hlt");
}
