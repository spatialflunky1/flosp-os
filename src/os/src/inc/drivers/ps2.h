#ifndef PS2_H
#define PS2_H

#include <drivers/io.h>
#include <kernel/message.h>
#include <kernel/timers.h>
#include <kernel/power_mgmt.h>

#define TRANSLATION_KEYBOARD 0
#define TRANSLATION_MOUSE    1
#define TRANSLATION_OTHER    2

typedef struct {
    ui8_t bytes_sent;
    ui8_t identity[2];
} ps2_port_identity;

int   ps2_init(void);
bool  is_ps2_init(void);
int   ps2_send_byte(ui8_t port, ui8_t b);
ui8_t ps2_pol_recieve_byte(void);
int   ps2_wait_for_ack(ui8_t resend_port, ui8_t resend_command);
ps2_port_identity ps2_port_identify(ui8_t port);
// Port device identity translations:
// 0: keyboard
// 1: mouse
// 2: other
ui8_t ps2_port_identity_translation(ps2_port_identity* port_identity);
bool ps2_check_error(void);
ps2_port_identity* get_ps2_port_identity(ui8_t port);

#endif
