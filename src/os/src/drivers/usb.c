#include <drivers/usb.h>

bool usb_initialized = false;

bool is_usb_init(void) {
    return usb_initialized;
}
