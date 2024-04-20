#include <kernel/timers.h>

// ticks: system uptime in hundreth seconds
// this method is used as it should be able to count for 5.8 billion years
ui64_t ticks = 0; 

// Sets the PIT channel 0 to a 100 Hz frequency
void pit_init(void) {
    // mode/command bitfield:
    // 6-7: Channel        (00b)
    // 4-5: Access Mode    (11b): lobyte then hibyte
    // 1-3: Operating Mode (011b): Mode 3 square wave generator
    // 0  : BCD            (0b)
    ui8_t command = 0b00110110;
    outb(PIT_MODE_COMMAND_REGISTER, command);

    // Calculate FREQUENCY_HZ Hz divisor
    int divisor = 1193180 / CHANNEL_0_FREQUENCY_HZ;

    // Set low byte
    outb(PIT_CHANNEL_0_DATA, divisor & 0xFF);
    
    // Set high byte
    outb(PIT_CHANNEL_0_DATA, divisor >> 8);
}

cpu_status_t* timer_handler(cpu_status_t* cpu_status) {
    ticks++;
    return cpu_status;
}

ui64_t get_uptime_seconds(void) {
    return ticks/100;
}

ui64_t get_uptime_ticks(void) {
    return ticks;
}
