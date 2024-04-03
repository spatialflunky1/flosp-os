#include <drivers/keyboard.h>

// All printable characters
// All null bytes are padding for keys that aren't printed
char chars[] = "\0001234567890-=\000"
               "\000qwertyuiop[]\n"
               "\000asdfghjkl;'`\000"
               "\\zxcvbnm,./\000\000"
               "\000 ";

char chars_cap[] = "\000!@#$%^&*()_+\000"
                   "\000QWERTYUIOP{}\n"
                   "\000ASDFGHJKL:\"\000"
                   "|ZXCVBNM<>?\000\000"
                   "\000 ";

// State variables
bool keyboard_enabled = false;
bool shift_down = false;

void keyboard_init(void) {
    kern_log(FILTER_DEBUG, "Initializing keyboard");
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
            shift_down = true;
            break;
        case 0xB6:
            shift_down = true;
        default:
            if (shift_down) {
                return chars_cap[keycode - 0x01];
            }
            else {
                return chars[keycode - 0x01];
            }
    }
    return 0;
}

void keyboard_int(void) {
    kputchar(keycode_char(inb(PS2_DATA_PORT)));
}

bool check_keyboard_enabled(void) {
    return keyboard_enabled;
}
