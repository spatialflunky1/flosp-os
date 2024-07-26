#include <drivers/ps2.h>

bool ps2_initialized = false;
bool dual_channel = false;
// -1: not set
bool port1 = -1;
bool port2 = -1;
bool ps2_error = false;

ps2_response identity1;
ps2_response identity2;

int ps2_init(void) {
    // Disable PS/2 ports
    kern_log(FILTER_DEBUG, "Debug: Disabling any possibly active PS/2 ports");
    ps2_send_cmd(PS2_DISABLE_PORT);
    ps2_send_cmd(PS2_DISABLE_PORT_2);

    // Flush output buffer
    flush_ps2_output_buffer();

    // Read for the configuration byte
    ui8_t config_byte = read_ps2_controller_config();

    // Check for dual channel
    if ((ui8_t)(config_byte & 0b00100000) == 1) {
        kern_log(FILTER_DEBUG, "Debug: Dual channel PS/2 controller found upon first check");
        dual_channel = true;
    }

    // Clear configuration byte bits 0,1,6
    // 0/1 : interrupts
    // 6   : port translation
    config_byte = config_byte & 0b10111100;
    write_ps2_controller_config(config_byte);
    read_ps2_controller_config();

    // Controller self test
    kern_log(FILTER_DEBUG, "Debug: Performing PS/2 controller self test");
    ps2_send_cmd(PS2_CONTROLLER_TEST);
    ui8_t response;
    while (true) {
        response = ps2_read_data();
        if (response == PS2_CONTROLLER_TEST_PASS) {
            kern_log(FILTER_DEBUG, "Debug: PS/2 controller test passed");
            break;
        }
        else if (response == PS2_CONTROLLER_TEST_FAIL) {
            kern_log(FILTER_ERROR, "Error: PS/2 controller test failed");
            return 1;
        }
    }

    // Second check for dual channel
    if (!dual_channel) {
        kern_log(FILTER_DEBUG, "Debug: Running secondary test for dual channel PS/2 controller");
        ps2_send_cmd(PS2_ENABLE_PORT_2);
        config_byte = read_ps2_controller_config();
        if ((ui8_t)(config_byte & 0b00100000) == 0) {
            dual_channel = true;
            kern_log(FILTER_DEBUG, "Debug: Dual channel PS/2 controller found upon second check");
            kern_log(FILTER_DEBUG, "Debug: Disabling PS/2 port 2");
            ps2_send_cmd(PS2_DISABLE_PORT_2);
        }
        else {
            kern_log(FILTER_DEBUG, "Debug: Dual channel PS/2 controller not detected");
        }
    }

    // Port self tests
    kern_log(FILTER_DEBUG, "Debug: Testing PS/2 port 1");
    ps2_send_cmd(PS2_TEST_PORT);
    while (port1 == -1) {
        response = ps2_read_data();
        switch (response) {
            case PS2_PORT_TEST_PASS:
                kern_log(FILTER_DEBUG, "Debug: PS/2 port 1 passed test");
                port1 = true;
                break;
            case PS2_PORT_TEST_FAIL_CL:
                kern_log(FILTER_WARNING, "PS/2 port 1 failed test: clock line struck low");
                port1 = false;
                break;
            case PS2_PORT_TEST_FAIL_CH:
                kern_log(FILTER_WARNING, "PS/2 port 1 failed test: clock line struck high");
                port1 = false;
                break;
            case PS2_PORT_TEST_FAIL_DL:
                kern_log(FILTER_WARNING, "PS/2 port 1 failed test: data line struck low");
                port1 = false;
                break;
            case PS2_PORT_TEST_FAIL_DH:
                kern_log(FILTER_WARNING, "PS/2 port 1 failed test: data line struck high");
                port1 = false;
                break;
        }
    }
    if (dual_channel) {
        kern_log(FILTER_DEBUG, "Debug: Testing PS/2 port 2");
        ps2_send_cmd(PS2_TEST_PORT);
        while (port2 == -1) {
            response = ps2_read_data();
            switch (response) {
                case PS2_PORT_TEST_PASS:
                    kern_log(FILTER_DEBUG, "Debug: PS/2 port 2 passed test");
                    port2 = true;
                    break;
                case PS2_PORT_TEST_FAIL_CL:
                    kern_log(FILTER_WARNING, "PS/2 port 2 failed test: clock line struck low");
                    port2 = false;
                    break;
                case PS2_PORT_TEST_FAIL_CH:
                    kern_log(FILTER_WARNING, "PS/2 port 2 failed test: clock line struck high");
                    port2 = false;
                    break;
                case PS2_PORT_TEST_FAIL_DL:
                    kern_log(FILTER_WARNING, "PS/2 port 2 failed test: data line struck low");
                    port2 = false;
                    break;
                case PS2_PORT_TEST_FAIL_DH:
                    kern_log(FILTER_WARNING, "PS/2 port 2 failed test: data line struck high");
                    port2 = false;
                    break;
            }
        }
    }

    // Enable ports
    if (!port1 && !port2) {
        kern_log(FILTER_ERROR, "No PS/2 ports avaliable");
        return 1;
    }
    if (port1) {
        kern_log(FILTER_DEBUG, "Debug: Enabling PS/2 port 1");
        ps2_send_cmd(PS2_ENABLE_PORT);
    }
    if (port2) {
        kern_log(FILTER_DEBUG, "Debug: Enabling PS/2 port 2");
        ps2_send_cmd(PS2_ENABLE_PORT_2);
    }

    // Enable interrupts
    config_byte = read_ps2_controller_config();
    if (port1) {
        kern_log(FILTER_DEBUG, "Debug: Enabling PS/2 interrupts on port 1");
        config_byte |= 0b00000001;
    }
    if (port2) {
        kern_log(FILTER_DEBUG, "Debug: Enabling PS/2 interrupts on port 2");
        config_byte |= 0b00000010;
    }
    write_ps2_controller_config(config_byte);
    read_ps2_controller_config();

    // Print identities
    kern_log(FILTER_DEBUG, "Debug: Getting PS/2 device identities");
    identity1 = ps2_port_identify(1);
    kprint("Debug: PS/2 port 1 identity: ");
    for (ui8_t i = 0; i < identity1.byte_count; i++) {
        // Second byte does not apply if first is one of these values
        if (i == 1 && 
           (identity1.data[i-1] == 0x00 ||
            identity1.data[i-1] == 0x03 ||
            identity1.data[i-1] == 0x04)) {
                kprint("N/A");
                break;
        }
        kprint_hex(identity1.data[i], 1);
        kprint(" ");
    }
    kprint("\n");
    identity2 = ps2_port_identify(2);
    kprint("Debug: PS/2 port 2 identity: ");
    for (ui8_t i = 0; i < identity2.byte_count; i++) {
        // Second byte does not apply if first is one of these values
        if (i == 1 && 
           (identity2.data[i-1] == 0x00 ||
            identity2.data[i-1] == 0x03 ||
            identity2.data[i-1] == 0x04)) {
                kprint("N/A");
                break;
            }
        kprint_hex(identity2.data[i], 1);
        kprint(" ");
    }
    kprint("\n");
    ps2_initialized = true;
    return 0;
}

void flush_ps2_output_buffer(void) {
    kern_log(FILTER_DEBUG, "Debug: Flushing PS/2 output buffer");
    while ((inb(PS2_STATUS_PORT) & 0b1) == 1) {
        inb(PS2_DATA_PORT);
    }
}

ui8_t read_ps2_controller_config(void) {
    flush_ps2_output_buffer();
    ps2_send_cmd(PS2_READ_CONFIG_BYTE);
    ui8_t config_byte = ps2_read_data(); 
    kprint("READ: PS/2 Controller Configuration: ");
    kprint_bin(config_byte, 1);
    kputchar('\n');
    return config_byte;
}

void write_ps2_controller_config(ui8_t config_byte) {
    kprint("WRITE: PS/2 Controller Configuration: ");
    kprint_bin(config_byte, 1);
    kputchar('\n');
    ps2_send_cmd(PS2_WRITE_CONFIG_BYTE);
    ps2_write_data(config_byte);
}

void ps2_send_cmd(ui8_t cmd) {
    // Wait to write
    while ((inb(PS2_STATUS_PORT) & 0b10) != 0); // byte 1: input buffer status
    // Write
    outb(PS2_COMMAND_PORT, cmd);
}

void ps2_write_data(ui8_t data) {
    // Wait to write
    while ((inb(PS2_STATUS_PORT) & 0b10) != 0); // byte 1: input buffer status
    // Write
    outb(PS2_DATA_PORT, data);
}

ui8_t ps2_read_data(void) {
    // Wait until ready for read
    while ((inb(PS2_STATUS_PORT) & 0b1) == 0); // byte 0: output buffer status (wait until data found)
    // Read
    return inb(PS2_DATA_PORT);
}

bool is_ps2_init(void) {
    return ps2_initialized;
}

int ps2_port_write_data(ui8_t port, ui8_t b) {
    // timeout variable
    ui64_t timeout_start;
    if (port == 2) {
        // Set controller to write to port 2 if set
        if (!port2) {
            kern_log(FILTER_WARNING, "Warning: PS/2 byte attempted to be sent to invalid port");
            return 1;
        }
        ps2_send_cmd(PS2_WRITE_BYTE_INPUT_PORT_2);
    }
    if (port == 1 && !port1) {
        kern_log(FILTER_WARNING, "Warning: PS/2 byte attempted to be sent to invalid port");
        return 1; 
    }
    timeout_start = get_uptime_ticks();
    while ((inb(PS2_STATUS_PORT) & 2) == 1) {
        if (get_uptime_ticks() >= timeout_start + PS2_TIMEOUT_DURATION) {
            kern_log(FILTER_ERROR, "Error: PS/2 timeout: input buffer full");
            return 1;
        }
    }
    ps2_write_data(b);
    return 0;
}

ui8_t ps2_pol_recieve_byte(void) {
    ui64_t timeout_start = get_uptime_ticks();
    while ((inb(PS2_STATUS_PORT) & 0b1) == 0) {
        if (get_uptime_ticks() >= timeout_start + PS2_TIMEOUT_DURATION) {
            kern_log(FILTER_WARNING, "Error: PS/2 polling timeout: output buffer still empty, no data recieved");
            ps2_error = true;
            return 1; // value does not matter
        }
    }
    return inb(PS2_DATA_PORT);
}

int ps2_wait_for_ack(ui8_t resend_port, ui8_t resend_command) {
    ui8_t response=0;
    ui64_t timeout_start = get_uptime_ticks();
    while (response = ps2_pol_recieve_byte(), response != PS2_ACK) {
        // ps2_error may be set when calling the above function
        if (ps2_check_error()) {
            return 1;
        }
        if (response == PS2_RESEND) {
            ps2_port_write_data(resend_port, resend_command);
        }
        if (get_uptime_seconds() >= timeout_start + PS2_TIMEOUT_DURATION) {
            kern_log(FILTER_ERROR, "Error: PS/2 timeout: command never aknowledged");
            return 1;
        }
    }
    return 0;
}

ps2_response ps2_port_identify(ui8_t port) {
    ps2_response response = {0, {0,0,0}};
    ui8_t response_byte;
    ui64_t timeout_start;
    // Disable scanning
    ps2_port_write_data(port, PS2_DISABLE_SCANNING);
    if (ps2_wait_for_ack(port, PS2_DISABLE_SCANNING) == 1) {
        ps2_error = true;
        return response;
    }
    // Identify device
    ps2_port_write_data(port, PS2_IDENTIFY);
    ps2_wait_for_ack(port, PS2_IDENTIFY);
    // Retrieve response
    // device will send up to 2 bytes (may be 0)
    while (response.byte_count != 2) {
        timeout_start = get_uptime_ticks();
        while ((inb(PS2_STATUS_PORT) & 1) == 0) {
            // 1 second timeout
            if (get_uptime_ticks() >= timeout_start + PS2_TIMEOUT_DURATION) {
                kern_log(FILTER_DEBUG, "Debug: Intentional PS/2 timeout: end of identity command");
                // Re-enable scanning
                ps2_port_write_data(port, PS2_ENABLE_SCANNING);
                ps2_wait_for_ack(port, PS2_ENABLE_SCANNING);
                return response;
            }
        }
        response_byte = ps2_pol_recieve_byte();
        if (ps2_check_error()) {
            return response;
        }
        response.data[response.byte_count] = response_byte;
        response.byte_count++;
    }
    // Re-enable scanning
    ps2_port_write_data(port, PS2_ENABLE_SCANNING);
    ps2_wait_for_ack(port, PS2_ENABLE_SCANNING);

    return response;
}

ui8_t ps2_port_identity_translation(ps2_response* port_identity) {
    // will most likely be subject to change
    if (port_identity->byte_count == 1) {
        return TRANSLATION_MOUSE; 
    }
    else if (port_identity->byte_count == 2) {
        return TRANSLATION_KEYBOARD;
    }
    else {
        return TRANSLATION_OTHER;
    }

}

// For functions that can't return an error code
// Clears error when checked
bool ps2_check_error(void) {
    if (ps2_error) {
        ps2_error = false;
        return true;
    }
    return false;
}

ps2_response* get_ps2_port_identity(ui8_t port) {
    if (port == 1) {
        return &identity1;
    }
    else if (port == 2) {
        return &identity2;
    }
    else {
        kern_log(FILTER_WARNING, "Warning: invalid port passed to `get_ps2_port_identity`");
        return NULL;
    }
}
