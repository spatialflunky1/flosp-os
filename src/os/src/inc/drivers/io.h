#ifndef IO_H
#define IO_H

#include <kernel/datatypes.h>

#define PS2_DATA_PORT 0x60

void outb(ui16_t port, ui8_t value);
ui8_t inb(ui16_t port);

#endif
