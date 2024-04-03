#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <kernel/datatypes.h>
#include <kernel/message.h>
#include <drivers/io.h>
#include <drivers/vga.h>

void keyboard_init(void);
char keycode_char(ui8_t keycode);
void keyboard_int(void);
bool check_keyboard_enabled(void);

#endif
