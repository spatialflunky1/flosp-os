#include <kernel/power_mgmt.h>

void cpu_freeze(void) {
    kern_log(FILTER_CRITICAL, "CPU Frozen");
    while (1) {
        __asm__ volatile ("cli; hlt");
    }
}
