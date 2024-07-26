#ifndef TIMERS_H
#define TIMERS_H

#include <drivers/io.h>
#include <cpu/cpu.h>
#include <cpu/acpi.h>
#include <cpu/apic.h>

#define CHANNEL_0_FREQUENCY_HZ 100
// HPET memory mapped registers
#define HPET_OFFSET_GENERAL_CAPABILITIES                  0x000
#define HPET_OFFSET_GENERAL_CONFIGURATION                 0x010
#define HPET_OFFSET_GENERAL_INTERRUPT_STATUS              0x020
#define HPET_OFFSET_MAIN_COUNTER_VALUE                    0x0F0
#define HPET_OFFSET_TIMER_N_CONFIGURATION_CAPABILITIES(N) (0x100 + 0x20) * N
#define HPET_OFFSET_TIMER_N_COMPARATOR_VALUE(N)           (0x108 + 0x20) * N
#define HPET_OFFSET_TIMER_N_FSB_INTERRUPT_ROUTE(N)        (0x110 + 0x20) * N
// HPET time units
#define HPET_SECONDS      1000000000000000
#define HPET_MILLISECONDS 1000000000000
#define HPET_FEMTOSECONDS 1

void timers_init(void);
void pit_disable(void);
void pit_init(void);
void hpet_init(hpet_t* HPET);
ui64_t poll_hpet(void);
void sleep_timer(ui32_t time_ms);
void hpet_sleep(ui64_t time, ui64_t time_units);
void lapic_timer_init(void);
cpu_status_t* timer_handler(cpu_status_t* cpu_status);
ui64_t get_uptime_seconds(void);
ui64_t get_uptime_ticks(void);

#endif
