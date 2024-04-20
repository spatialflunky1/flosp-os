#include <drivers/keyboard.h>

// default is -1 to define no port
int ps2_port = -1;

// All printable characters
// All null bytes are padding for keys that aren't printed
char chars[] = "\000\0001234567890-=\000"
               "\000qwertyuiop[]\n"
               "\000asdfghjkl;'`\000"
               "\\zxcvbnm,./\000\000"
               "\000 ";
char chars_cap[] = "\000\000!@#$%^&*()_+\000"
                   "\000QWERTYUIOP{}\n"
                   "\000ASDFGHJKL:\"~\000"
                   "|ZXCVBNM<>?\000\000"
                   "\000 ";

// State variables
bool keyboard_enabled = false;
bool shift_down = false;

int ps2_scancode_correction(ui8_t port) {
    if (ps2_send_byte(port, PS2_KEYBOARD_SET_SCANCODE_SET) == 1) {
        return 1;
    }
    if (ps2_wait_for_ack(port, PS2_KEYBOARD_SET_SCANCODE_SET) == 1) {
        return 1;
    }
    if (ps2_send_byte(port, PS2_KEYBOARD_SET_SCANCODE_1) == 1) {
        return 1;
    }
    if (ps2_wait_for_ack(port, PS2_KEYBOARD_SET_SCANCODE_1) == 1) {
        return 1;
    }
    return 0;
}

void keyboard_init(void) {
    kern_log(FILTER_DEBUG, "Debug: Initializing keyboard");
    if (is_ps2_init()) {
        // check and set ps/2 keyboard port info
        if (ps2_port_identity_translation(get_ps2_port_identity(1)) == TRANSLATION_KEYBOARD) {
            if (ps2_scancode_correction(1) == 1) {
                return;
            }
        }
        else if (ps2_port_identity_translation(get_ps2_port_identity(1)) == TRANSLATION_KEYBOARD) {
            if (ps2_scancode_correction(2) == 1) {
                return;
            }
        }
        else {
            kern_log(FILTER_ERROR, "No PS/2 keyboard detected");
            return;
        }
    }
    keyboard_enabled = true;
}

char keycode_char(ui8_t keycode) {
    switch (keycode) {
        case 0x2A:
            shift_down = true;
            break;
        case 0x36:
            shift_down = true;
            break;
        case 0xAA:
            shift_down = false;
            break;
        case 0xB6:
            shift_down = false;
        default:
            if (shift_down) {
                return chars_cap[keycode];
            }
            else {
                return chars[keycode];
            }
    }
    return 0;
}

void keyboard_handler(void) {
    kputchar(keycode_char(inb(PS2_DATA_PORT)));
}

bool check_keyboard_enabled(void) {
    return keyboard_enabled;
}
