#include <kernel/timers.h>

// ticks: system uptime in 10 milliseconds
// this method is used as it should be able to count for a little over 5840 million years
volatile ui64_t ticks = 0;

// HPET vars
ui16_t hpet_minimum_tick;
ui32_t hpet_period;
// HPET address
volatile void*   hpet_base_addr      = NULL;
volatile ui64_t* hpet_cfg_addr       = NULL;
volatile ui64_t* hpet_main_cntr_addr = NULL;
// LAPIC
bool lapic_timer_enabled = false;

// Detects and sets system timers
void timers_init(void) {
    acpi_tables_t* acpi_tables = get_acpi_tables();
    kern_log(FILTER_DEBUG, "Debug: Initializing the lapic timer");
    if (acpi_tables->HPET != NULL) { 
        kern_log(FILTER_DEBUG, "Debug: Initializing the HPET");
        hpet_init(acpi_tables->HPET);
    }
    if (hpet_base_addr != NULL) {
        kern_log(FILTER_DEBUG, "Debug: Using the HPET to calibrate the lapic timer");
        kern_log(FILTER_DEBUG, "Debug: Disabling the PIT");
        pit_disable();
    }
    else {
        kern_log(FILTER_DEBUG, "Debug: Using the PIT to calibrate the lapic timer");
        kern_log(FILTER_DEBUG, "Debug: Initializing the PIT");
        pit_init();
    }
    kern_log(FILTER_DEBUG, "Debug: Initializing the lapic timer");
    lapic_timer_init();
}

void pit_disable(void) {
    // mode/command bitfield:
    // 6-7: Channel        (00b)
    // 4-5: Access Mode    (11b): lobyte then hibyte
    // 1-3: Operating Mode (001b): Mode 1 Hardware Re-triggerable One-shot
    // 0  : BCD            (0b)
    ui8_t command = 0b00110010;
    outb(PIT_MODE_COMMAND_REGISTER, command);
}

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

void hpet_init(hpet_t* HPET) {
    // Already determined if HPET is null, no need for a second check
    hpet_base_addr = (void*)HPET->Base_Address.Address;
    kprint("Debug: HPET Address: ");
    kprint_hex((ui64_t)hpet_base_addr, 1);
    kprint("\n"); 
    if (hpet_base_addr == NULL) {
        kern_log(FILTER_ERROR, "Error: HPET address is NULL");
        return;
    }
    if (HPET->Base_Address.AddressSpace != ACPI_MEMORY_SPACE) {
        kern_log(FILTER_ERROR, "Error: HPET is not located within memory");
        hpet_base_addr = NULL;
        return;
    }
    if (HPET->sdt_header.revision == 0) {
        kern_log(FILTER_ERROR, "Error: HPET revision is 0");
        hpet_base_addr = NULL;
        return;
    }
    hpet_cfg_addr       = hpet_base_addr + HPET_OFFSET_GENERAL_CONFIGURATION;
    hpet_main_cntr_addr = hpet_base_addr + HPET_OFFSET_MAIN_COUNTER_VALUE;
    // Get the timer period (in femtoseconds)
    hpet_period = (*(ui64_t*)hpet_base_addr) >> 32;
    // Clear bit 1 (disable legacy replacement, stop PIT emulation)
    *hpet_cfg_addr = (*hpet_cfg_addr) & 0xFFFFFFFD; 
    // Set bit 0 (enable main counter)
    *hpet_cfg_addr = (*hpet_cfg_addr) | 0x00000001;
    kern_log(FILTER_DEBUG, "Debug: HPET initialized");
}

// Value returned in femtoseconds
ui64_t poll_hpet(void) {
    return *hpet_main_cntr_addr * hpet_period;
}

// Can be used by any timer connected to INT 0x20
void sleep_timer(ui32_t time_ms) {
    ui64_t end_time = ticks + time_ms;
    while (ticks < end_time);
}

// Need a separate HPET sleep function as it will not be connected to INT 0x20
void hpet_sleep(ui64_t time, ui64_t time_units) {
    ui64_t end_time = poll_hpet() + (time*time_units);
    while (poll_hpet() < end_time);
}

// LAPIC timer will interrupt every 10 ms (100 Hz)
void lapic_timer_init(void) {
    volatile void* lapic_addr = get_lapic_address();
    // APIC timer set to divider 16
    lapic_write_reg((ui64_t)lapic_addr + APIC_TIMER_DIVIDER, 0x3);
    // Easiest way to check if HPET is initialized is by checking the saved address
    // Repeated code in if statement to avoid delay in calibration
    if (hpet_base_addr != NULL) {
        // Set timer initial count to -1
        lapic_write_reg((ui64_t)lapic_addr + APIC_TIMER_INITIAL_COUNT, -1); // val: 0xFFFFFFFF
        // HPET sleep 10ms
        kern_log(FILTER_DEBUG, "Debug: HPET sleeping 10 ms for lapic calibration");
        hpet_sleep(10, HPET_MILLISECONDS);
    }
    else {
        // Set timer initial count to -1
        lapic_write_reg((ui64_t)lapic_addr + APIC_TIMER_INITIAL_COUNT, -1); // val: 0xFFFFFFFF
        // PIT sleep 10ms
        kern_log(FILTER_DEBUG, "Debug: PIT sleeping for 10 ms for lapic calibration");
        sleep_timer(10);
    }
    kern_log(FILTER_DEBUG, "Debug: lapic timer calibration complete");
    
    // Stop apic timer
    lapic_write_reg((ui64_t)lapic_addr + APIC_LVT_TIMER, APIC_DISABLE);
    // Save amount of ticks in 10 ms
    ui32_t ticks_10ms = 0xFFFFFFFF - lapic_read_reg((ui64_t)lapic_addr + APIC_TIMER_CURRENT_COUNT);
    //
    // Re-enable the timer with the found value
    //
    kern_log(FILTER_DEBUG, "Debug: Enabling the lapic timer");
    // Set timer periodic on IRQ 0 (int 0x20)
    lapic_write_reg((ui64_t)lapic_addr + APIC_LVT_TIMER, 0x20 | APIC_TIMER_MODE_PERIODIC);
    // APIC timer set to divider 16
    lapic_write_reg((ui64_t)lapic_addr + APIC_TIMER_DIVIDER, lapic_read_reg((ui64_t)lapic_addr + APIC_TIMER_DIVIDER) | 0x3);
    // Set timer with number of ticks found
    lapic_write_reg((ui64_t)lapic_addr + APIC_TIMER_INITIAL_COUNT, ticks_10ms);
    lapic_timer_enabled = true;
    kern_log(FILTER_DEBUG, "Debug: lapic timer initialized");
}

cpu_status_t* timer_handler(cpu_status_t* cpu_status) {
    if (lapic_timer_enabled) {
        ticks++;
        lapic_write_reg((ui64_t)get_lapic_address() + APIC_EOI, 0);
    }
    return cpu_status;
}

ui64_t get_uptime_seconds(void) {
    return ticks/100;
}

ui64_t get_uptime_ticks(void) {
    return ticks;
}
