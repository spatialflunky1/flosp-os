#ifndef KERNEL_H
#define KERNEL_H

#include <drivers/vga.h>
#include <kernel/idt.h>

#ifndef KVER
#define KVER "0.0.0"
#endif

#ifndef DEBUG_FILTER
#define DEBUG_FILTER 0
#endif
#define FILTER_NONE     0
#define FILTER_INFO     1
#define FILTER_WARNING  2
#define FILTER_ERROR    3
#define FILTER_CRITICAL 4
#define FILTER_ALL      5

#endif
