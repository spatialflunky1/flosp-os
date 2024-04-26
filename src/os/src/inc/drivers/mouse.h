#ifndef MOUSE_H
#define MOUSE_H

#include <datatypes.h>
#include <drivers/ps2.h>

bool check_mouse_enabled(void);
void mouse_handler(void);

#endif
