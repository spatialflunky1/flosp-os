#include <drivers/mouse.h>

bool mouse_enabled = false;

bool check_mouse_enabled(void) {
    return mouse_enabled;
}

void mouse_handler(void) {
    // empty for now, mouse not implemented yet
    return;
}
