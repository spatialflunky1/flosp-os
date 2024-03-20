#include <drivers/mem.h>

void memcpy(ui8_t* source, ui8_t* dest, ui32_t nbytes) {
    for (int i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}
