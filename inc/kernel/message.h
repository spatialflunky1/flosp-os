#ifndef MESSAGE_H
#define MESSAGE_H

#include <kernel/datatypes.h>
#include <drivers/vga.h>

#ifndef MESSAGE_FILTER
#define MESSAGE_FILTER 0
#endif
#define FILTER_NONE     0
#define FILTER_INFO     1
#define FILTER_WARNING  2
#define FILTER_ERROR    3
#define FILTER_CRITICAL 4

void kern_log(ui8_t filter, const char* msg);

#endif
