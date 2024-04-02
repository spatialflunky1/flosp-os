#include <drivers/mem.h>

void memcpy(void* sourcep, void* destp, ui32_t nbytes) {
    ui8_t* source = (ui8_t*)sourcep;
    ui8_t* dest   = (ui8_t*)destp;
    for (ui32_t i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}
