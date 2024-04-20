#ifndef TIMERS_H
#define TIMERS_H

#include <drivers/io.h>
#include <cpu/cpu.h>

#define CHANNEL_0_FREQUENCY_HZ 100
#define PS2_TIMEOUT_DURATION 1

void pit_init(void);
cpu_status_t* timer_handler(cpu_status_t* cpu_status);
ui64_t get_uptime_seconds(void);
ui64_t get_uptime_ticks(void);

#endif
