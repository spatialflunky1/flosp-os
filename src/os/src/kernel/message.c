#include <kernel/message.h>

void kern_log(ui8_t filter, const char* msg) {
    if (MESSAGE_FILTER < filter) {
        kprint(msg);
        kprint("\n");
    }
}
