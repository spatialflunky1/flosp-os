#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <datatypes.h>
#include <kernel/message.h>
#include <drivers/io.h>
#include <drivers/vga.h>
#include <drivers/ps2.h>
#include <cpu/acpi.h>
#include <drivers/usb.h>

void keyboard_init(void);
char keycode_char(ui8_t keycode);
int  ps2_scancode_correction(ui8_t port);
void keyboard_handler(void);
bool check_keyboard_enabled(void);

#endif
