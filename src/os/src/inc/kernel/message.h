#ifndef MESSAGE_H
#define MESSAGE_H

#include <kernel/datatypes.h>
#include <drivers/vga.h>

#define FILTER_NONE     0
#define FILTER_DEBUG    1
#define FILTER_INFO     2
#define FILTER_WARNING  3
#define FILTER_ERROR    4
#define FILTER_CRITICAL 5

#ifndef MESSAGE_FILTER
#define MESSAGE_FILTER FILTER_NONE
#endif

void kern_log(ui8_t filter, const char* msg);

#endif
